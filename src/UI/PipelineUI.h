#pragma once

/*
 * PipelineUI.h
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */

#include <iostream>
#include <string>
#include <vector>

#include "GtkHeader.h"

#include <iostream>
#include <BackTracer.h>

#include "../MandelPipeline/MandelPipelineWrapper.h"
#include "../Platform/CL/CLFactory.h"
#include "../Platform/CPU/CPUFactory.h"
#include "../UI/PipelineParameterBox.h"
#include "MandelbrotImageView.h"

struct AbstractPlatform{
	virtual PipelineWrapper& getPipeline() = 0;
	virtual PipelineParameterBox& getParameterBox() = 0;
	virtual const CPUImage<unsigned>& getRenderedImage() = 0;

	virtual void scale(float factor) = 0;
	virtual void translate(float x,float y) = 0;
	virtual std::string getName() const = 0;

	virtual ~AbstractPlatform() = default;
};

template<typename Factory,typename T>
struct Platform : AbstractPlatform{
	Platform(Factory factory):
		factory(factory){}

	PipelineWrapper& getPipeline(){
		if(!pipeline){
			pipeline = std::make_unique<MandelPipelineWrapper<Factory,T>>(factory,demangle(typeid(T)));
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
		auto& renderedImage = pipeline->getRenderedImage();
		if(outputImage.getHeight() != renderedImage.getHeight() ||
		   outputImage.getWidth() != renderedImage.getWidth()){
			outputImage = CPUImage<unsigned>(renderedImage.getWidth(),renderedImage.getHeight());
		}
		renderedImage.copyToBuffer(outputImage.getDataBuffer());
		return outputImage;
	}

	void scale(float factor){
		//get current scale.
		auto& param = this->pipeline->getPositionParam();
		auto scale = param.template getValue<2>();

		param.template getParam<2>().setValue(tmul(scale,fromFloatToType<decltype(scale)>(factor)));
	}

	//translate by x,y in image space
	void translate(float x,float y){
		//get current position and scale.
		auto& param = this->pipeline->getPositionParam();
		auto offsetReal = param.template getValue<0>();
		auto offsetImag = param.template getValue<1>();
		auto scale = param.template getValue<2>();

		param.template getParam<0>().setValue(tadd(offsetReal,tmul(scale,fromFloatToType<decltype(scale)>(x))));
		param.template getParam<1>().setValue(tadd(offsetImag,tmul(scale,fromFloatToType<decltype(scale)>(y))));
	}

	std::string getName() const {
		return factory.getDeviceName();
	}
protected:
	CPUImage<unsigned> outputImage{1,1};
	Factory factory;
	std::unique_ptr<MandelPipelineWrapper<Factory,T>> pipeline = nullptr;
	std::unique_ptr<PipelineParameterBox> paramBox = nullptr;
};

struct MainWindow : Gtk::Window{
	MainWindow();

	AbstractPlatform* getSelectedPlatform(){
		return selectedPlatform;
	}

	void calculateNow();
protected:
	Gtk::HeaderBar header;
	Gtk::Box verticalBox{Gtk::ORIENTATION_VERTICAL};
	Gtk::Paned mainView;
	MandelbrotImageView imageView;

	Gtk::Button calculateButton{"calculate"};
	Gtk::ComboBoxText platformBox;
	Gtk::ComboBoxText typeBox;

	Gtk::Box parameterBox;

	std::list<std::unique_ptr<AbstractPlatform>> platforms;
	std::map<std::pair<std::string,std::string>,AbstractPlatform*> platformMap;

	AbstractPlatform* selectedPlatform = nullptr;

	void loadPlatform();
};



