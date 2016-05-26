#pragma once
#include "MandelPipelineWrapper.h"

/*
 * MandelPipelineWrapper.inl
 *
 *  Created on: May 10, 2016
 *      Author: tly
 */

template<typename Factory,typename T>
MandelPipelineWrapper<Factory,T>::MandelPipelineWrapper(Factory& factory,const std::string& typeName)
	:factory(factory),
	 position(this),
	 calculation(this),
	 coloring(this),
	 reduction(this){
	_log("[info] creating pipeline with type " << typeName << " and platform " << factory.getDeviceName());

	sizeParam.setValue<0>(512);//width
	sizeParam.setValue<1>(512);//height

	sizeParam.output(0,1) >> imageRangeGenerator.input(0,1);
	imageRangeGenerator.getInput<2>().setDefaultValue(1);

	multiSamplingParam.setValue<0>(false); //enable multisampling
	multiSamplingParam.setValue<1>(2); //multisampling size
	multiSamplingParam.setValue<2>(0); //multisampling pattern
	multiSamplingParam.output(1,0) >> multisampleRangeGenerator.input(0,1);

	imageRangeGenerator.output(0) >> multisampleSizeParam.input(0);
	multisampleRangeGenerator.output(0) >> multisampleSizeParam.input(1);
	multiSamplingParam.output(0) >> multisampleSizeParam.input(2);

	multiSamplingParam.output(0,1,2) >> toStringAction.input(2,3,4);

	generalParam.template setValue<0>("z*z + c");
	generalParam.template setValue<1>(false);
	generalParam.template getParam<2>().setValueFromString("0");
	generalParam.template getParam<3>().setValueFromString("0");

	generalParam.output(0) >> formulaParser.input(0);
	generalParam.output(1) >> toStringAction.input(7);

	toStringAction.output(0,1,2,3,4,5,6,7) >> kernelDefinesAction.input(0,1,3,4,5,7,8,9);
	kernelDefinesAction.template getInput<2>().setDefaultValue(typeName);
	formulaParser.output(0) >> kernelDefinesAction.input(6);
	this->addParam(generalParam);
	this->addParam(sizeParam);
	this->addParam(multiSamplingParam);
}

template<typename Factory,typename T>
void MandelPipelineWrapper<Factory,T>::run(){
	_log("[info] running pipeline " + demangle(typeid(*this)));
	Timer timer;

	timer.start();
	reduction.reductionKernel.run();
	auto fullTime = timer.stop();
	_log("[info] position: " << position.positionKernel.template getOutput<1>().getValue());
	_log("[info] calculation: " << calculation.mandelbrotKernel.template getOutput<1>().getValue());
	_log("[info] coloring: " << coloring.coloringKernel.template getOutput<1>().getValue());
	_log("[info] reduction: " << reduction.reductionKernel.template getOutput<1>().getValue());
	_log("[info] full: " << fullTime << " us.");
}

template<typename Factory,typename T>
const typename MandelPipelineWrapper<Factory,T>::U32Image& MandelPipelineWrapper<Factory,T>::getRenderedImage(){
	return reduction.reductionKernel.template getOutput<0>().getValue();
}

template<typename Factory,typename T>
UIParameterAction<T,T,T,float>& MandelPipelineWrapper<Factory,T>::getPositionParam(){
	return position.positionParam;
}




