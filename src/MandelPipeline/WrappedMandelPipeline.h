#pragma once

#include "../Actions/GeneratorAction.h"
#include "../Actions/ImageGeneratorAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Type/Vec.h"
#include "../UI/UIParameterAction.h"
#include "PipelineWrapper.h"
#include "../Type/TypeHelper.h"
#include "KernelDefinesAction.h"
#include "ToStringAction.h"
#include "../Actions/FunctionCallAction.h"

/*
 * WrappedMandelPipeline.h
 *
 *  Created on: 03.05.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct PositionWrapper;
template<typename Factory,typename T> struct CalculationWrapper;
template<typename Factory,typename T> struct ReductionWrapper;
template<typename Factory,typename T> struct ColoringWrapper;

template<typename Factory,typename T>
struct WrappedMandelPipeline : PipelineWrapper{
	using U32Image = typename Factory::template Image<uint32_t>;
	using FloatImage = typename Factory::template Image<float>;
	using ComplexImage = typename Factory::template Image<Vec<2,T>>;

	WrappedMandelPipeline(Factory& factory,const std::string& typeName):
		factory(factory){
		_log("[info] creating pipeline with type " << typeName << " and platform " << factory.getDeviceName());

		sizeParam.setValue<0>(512);//width
		sizeParam.setValue<1>(512);//height

		sizeParam.output(0,1) >> imageRangeGenerator.input(0,1);
		imageRangeGenerator.getInput<2>().setDefaultValue(1);

		multiSamplingParam.setValue<0>(false); //enable multisampling
		multiSamplingParam.setValue<1>(2); //multisampling size
		multiSamplingParam.setValue<2>(0); //multisampling pattern
		multiSamplingParam.output(1,1) >> multisampleRangeGenerator.input(0,1);

		imageRangeGenerator.output(0) >> multisampleSizeParam.input(0);
		multisampleRangeGenerator.output(0) >> multisampleSizeParam.input(1);
		multiSamplingParam.output(0) >> multisampleSizeParam.input(2);

		iterationParam.output(0,1) >> toStringAction.input(0,1);
		multiSamplingParam.output(0,1,2) >> toStringAction.input(2,3,4);

		toStringAction.output(0,1) >> kernelDefinesAction.input(0,1);
		toStringAction.output(2,3,4) >> kernelDefinesAction.input(3,4,5);
		kernelDefinesAction.template getInput<2>().setDefaultValue(typeName);
	}

	virtual void run(){
		_log("[info] running pipeline " + demangle(typeid(*this)));
		Timer timer;

		timer.start();
		coloringKernel.run();
		auto fullTime = timer.stop();
		_log("[info] position: " << positionKernel.template getOutput<1>().getValue());
		_log("[info] calculation: " << mandelbrotKernel.template getOutput<1>().getValue());
		_log("[info] coloring: " << coloringKernel.template getOutput<1>().getValue());
		_log("[info] full: " << fullTime << " us.");
	}

	const U32Image& getRenderedImage(){
		return coloringKernel.template getOutput<0>().getValue();
	}

	UIParameterAction<T,T,T>& getPositionParam(){
		return positionParam;
	}
protected:
	Factory& factory;

	friend struct PositionWrapper<Factory,T>;
	friend struct CalculationWrapper<Factory,T>;
	friend struct ReductionWrapper<Factory,T>;
	friend struct ColoringWrapper<Factory,T>;

	//general actions:
	//size of the generated image
	UIParameterAction<uint32_t,uint32_t> sizeParam{"image dimensions","width","height"};
	UIParameterAction<bool,uint32_t,uint32_t> multiSamplingParam{"multisampling", "enable", "size" ,"pattern"};
	//generates a Range object, fitting the sizeParam
	GeneratorAction<Input(uint32_t,uint32_t,uint32_t),Output(Range)> imageRangeGenerator;
	GeneratorAction<Input(uint32_t,uint32_t),Output(Range)> multisampleRangeGenerator;
	FunctionCallAction<Input(Range,Range,bool),Range> multisampleSizeParam{
		[](const Range& imageRange,const Range& msRange, const bool& msEnabled)->Range{
			return msEnabled ? Range{
				imageRange.x * msRange.x,
				imageRange.y * msRange.y
			} : imageRange;
		}
	};
	//generates defines used to compile the kernels
	KernelDefinesAction<6> kernelDefinesAction{
		"MAXITER","BAILOUT","Type",
		"MULTISAMPLING_ENABLED","MULTISAMPLING_SIZE","MULTISAMPLING_PATTERN"
	};
	//generates strings for the defines
	ToStringAction<uint32_t,float,bool,uint32_t,uint32_t> toStringAction;
};


