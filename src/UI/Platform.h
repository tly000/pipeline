#pragma once
#include "../MandelPipeline/PipelineWrapper.h"
#include "PipelineParameterBox.h"
#include <list>
#include <string>
#include "../Platform/CPU/CPUImage.h"
#include "../MandelPipeline/MandelPipeline.h"
#include "../Platform/CPU/CPUFactory.h"
#include "../Platform/CL/CLFactory.h"
#include "../tests/ExternTemplates.h"

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
		auto& renderedImage = pipeline->reductionAction.getOutput(_C("reducedImage")).getValue();
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
		auto scale = param.getValue(_C("scale"));

		param.setValue(_C("scale"), tmul(scale,fromFloatToType<decltype(scale)>(factor)));
	}

	//translate by x,y in image space
	void translate(float x,float y){
		//get current position and scale.
		auto& param = this->pipeline->positionParams;
		auto offsetReal = param.getValue(_C("center real"));
		auto offsetImag = param.getValue(_C("center imag"));
		auto scale = param.getValue(_C("scale"));
		float rotationAngle = param.getValue(_C("angle")) / 180 * G_PI;

		auto cosA = fromFloatToType<decltype(scale)>(std::cos(rotationAngle));
		auto sinA = fromFloatToType<decltype(scale)>(std::sin(rotationAngle));
		auto X = tmul(scale,fromFloatToType<decltype(scale)>(x));
		auto Y = tmul(scale,fromFloatToType<decltype(scale)>(y));
		auto newX = tsub(tmul(X,cosA),tmul(Y,sinA));
		auto newY = tadd(tmul(X,sinA),tmul(Y,cosA));

		param.setValue(_C("center real"),tadd(offsetReal,newX));
		param.setValue(_C("center imag"),tadd(offsetImag,newY));
	}

	//translate by x,y in image space
	void rotate(float angle){
		//get current position and scale.
		auto& param = this->pipeline->positionParams;
		float currentAngle = param.getValue(_C("angle"));
		param.setValue(_C("angle"), currentAngle + angle);
	}

	std::string getName() const {
		return factory.getDeviceName();
	}

	bool isDone() const{
		return this->pipeline->calcAction.getOutput(_C("done")).getValue();
	}
	void setReset(bool enable){
		this->pipeline->calcAction.getInput(_C("reset calculation")).setDefaultValue(enable);
	}
protected:
	std::string typeName;
	CPUImage<unsigned> outputImage{1,1};
	Factory factory;
	std::unique_ptr<MandelPipeline<Factory,T>> pipeline = nullptr;
	std::unique_ptr<PipelineParameterBox> paramBox = nullptr;
};

extern template struct Platform<CPUFactory<true>,float>;
extern template struct Platform<CPUFactory<true>,double>;
//extern template struct Platform<CPUFactory,longdouble>;
//#ifdef QUADMATH_H
//extern template struct Platform<CPUFactory,float128>;
//#endif
extern template struct Platform<CPUFactory<true>,Fixed4>;
//extern template struct Platform<CPUFactory,Fixed8>;
//extern template struct Platform<CPUFactory,Fixed16>;

extern template struct Platform<CLFactory,float>;
extern template struct Platform<CLFactory,double>;
extern template struct Platform<CLFactory,Fixed4>;
extern template struct Platform<CLFactory,Fixed8>;
extern template struct Platform<CLFactory,Fixed16>;
