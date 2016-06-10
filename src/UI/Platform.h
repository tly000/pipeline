#pragma once
#include "../MandelPipeline/PipelineWrapper.h"
#include "PipelineParameterBox.h"
#include <list>
#include <string>
#include "../Platform/CPU/CPUImage.h"
#include "../MandelPipeline/MandelPipeline.h"
#include "../Platform/CPU/CPUFactory.h"
#include "../Platform/CL/CLFactory.h"

/*
 * AbstractPlatform.h
 *
 *  Created on: 02.06.2016
 *      Author: tly
 */

struct AbstractPlatform{
	virtual PipelineWrapper& getPipeline() = 0;
	virtual PipelineParameterBox& getParameterBox() = 0;
	virtual const CPUImage<unsigned>& getRenderedImage() = 0;

	virtual void scale(float factor) = 0;
	virtual void translate(float x,float y) = 0;
	virtual void rotate(float angle) = 0;
	virtual std::string getName() const = 0;
	virtual bool isDone() const = 0;
	virtual void setReset(bool enable) = 0;

	virtual std::string paramsToJson(const std::map<std::string,std::list<std::string>>& paramsToSave)= 0;
	virtual void paramsFromJson(const std::string& jsonObj)= 0;

	virtual ~AbstractPlatform() = default;
};

template<typename Factory,typename T>
struct Platform : AbstractPlatform{
	Platform(Factory factory,std::string typeName):
		typeName(typeName),
		factory(factory){}

	PipelineWrapper& getPipeline(){
		if(!pipeline){
			pipeline = std::make_unique<MandelPipeline<Factory,T>>(factory,typeName);
		}
		return *pipeline;
	}

	PipelineParameterBox& getParameterBox(){
		if(!paramBox){
			paramBox = std::make_unique<PipelineParameterBox>(this->getPipeline());
		}
		return *paramBox;
	}

	const CPUImage<unsigned>& getRenderedImage(){
		auto& renderedImage = pipeline->reductionAction.getOutput("reducedImage"_c).getValue();
		if(outputImage.getHeight() != renderedImage.getHeight() ||
		   outputImage.getWidth() != renderedImage.getWidth()){
			outputImage = CPUImage<unsigned>(renderedImage.getWidth(),renderedImage.getHeight());
		}
		renderedImage.copyToBuffer(outputImage.getDataBuffer());
		return outputImage;
	}

	void scale(float factor){
		//get current scale.
		auto& param = this->pipeline->positionParams;
		auto scale = param.template getValue("scale"_c);

		param.setValue("scale"_c, tmul(scale,fromFloatToType<decltype(scale)>(factor)));
	}

	//translate by x,y in image space
	void translate(float x,float y){
		//get current position and scale.
		auto& param = this->pipeline->positionParams;
		auto offsetReal = param.template getValue("center real"_c);
		auto offsetImag = param.template getValue("center imag"_c);
		auto scale = param.template getValue("scale"_c);
		float rotationAngle = param.template getValue("angle"_c) / 180 * G_PI;

		auto cosA = fromFloatToType<decltype(scale)>(std::cos(rotationAngle));
		auto sinA = fromFloatToType<decltype(scale)>(std::sin(rotationAngle));
		auto X = tmul(scale,fromFloatToType<decltype(scale)>(x));
		auto Y = tmul(scale,fromFloatToType<decltype(scale)>(y));
		auto newX = tsub(tmul(X,cosA),tmul(Y,sinA));
		auto newY = tadd(tmul(X,sinA),tmul(Y,cosA));

		param.setValue("center real"_c,tadd(offsetReal,newX));
		param.setValue("center imag"_c,tadd(offsetImag,newY));
	}

	//translate by x,y in image space
	void rotate(float angle){
		//get current position and scale.
		auto& param = this->pipeline->positionParams;
		float currentAngle = param.getValue("angle"_c);
		param.setValue("angle"_c, currentAngle + angle);
	}

	std::string getName() const {
		return factory.getDeviceName();
	}

	std::string paramsToJson(const std::map<std::string,std::list<std::string>>& paramsToSave){
		std::stringstream str;

		bool firstPack = true;
		str << "{\n";
		for(auto& pack : paramsToSave){
			if(!firstPack){
				str << ",\n";
			}else{
				firstPack = false;
			}
			str << "\t\"" << pack.first << "\"" << " : {\n";
			ParamPack& p = this->getPipeline().getParamPack(pack.first);

			bool firstParam = true;
			for(auto& paramName : pack.second){
				if(!firstParam){
					str << ",\n";
				}else{
					firstParam = false;
				}
				Parameter& param = p.getParam(paramName);
				str << "\t\t\"" << param.name << "\"" << " : " << "\"" << param.getValueAsString() << "\"";
			}
			str << "\n\t}";
		}
		str << "\n}";
		return str.str();
	}

	void paramsFromJson(const std::string& jsonObj){
		ParseTree obj = JsonParser::parseJson(jsonObj);
		_log("[info] json: " << printTree(obj));
		auto unquote = [](const std::string& s){
			return s.substr(1,s.size()-2);
		};

		assertOrThrow(obj.elementName == "object");
		for(auto& child : obj.children){
			assertOrThrow(child.elementName == "pair" && child.children.back().elementName == "object");
			std::string paramPackName = unquote(child.children.front().children.front().elementName);
			ParamPack& pack = this->getPipeline().getParamPack(paramPackName);
			auto& object = child.children.back();

			for(auto& param : object.children){
				std::string name = unquote(param.children.front().children.front().elementName);
				if(pack.hasParam(name)){
					auto& value = param.children.back();
					assertOrThrow(value.elementName == "string");
					assertOrThrow(value.elementName == "string");
					if(!pack.getParam(name).setValueFromString(unquote(value.children.front().elementName))){
						throw std::runtime_error("invalid value for parameter " + name);
					}
				}else{
					throw std::runtime_error("could not find parameter with name " + name + " inside pack with name " + paramPackName);
				}
			}
		}
	}

	bool isDone() const{
		return this->pipeline->calcAction.getOutput("done"_c).getValue();
	}
	void setReset(bool enable){
		this->pipeline->calcAction.getInput("reset calculation"_c).setDefaultValue(enable);
	}
protected:
	std::string typeName;
	CPUImage<unsigned> outputImage{1,1};
	Factory factory;
	std::unique_ptr<MandelPipeline<Factory,T>> pipeline = nullptr;
	std::unique_ptr<PipelineParameterBox> paramBox = nullptr;
};

extern template struct Platform<CPUFactory,float>;
extern template struct Platform<CPUFactory,double>;
extern template struct Platform<CPUFactory,longdouble>;
#ifdef QUADMATH_H
extern template struct Platform<CPUFactory,float128>;
#endif
extern template struct Platform<CPUFactory,Fixed4>;
extern template struct Platform<CPUFactory,Fixed8>;
extern template struct Platform<CPUFactory,Fixed16>;

extern template struct Platform<CLFactory,float>;
extern template struct Platform<CLFactory,double>;
extern template struct Platform<CLFactory,Fixed4>;
extern template struct Platform<CLFactory,Fixed8>;
extern template struct Platform<CLFactory,Fixed16>;
