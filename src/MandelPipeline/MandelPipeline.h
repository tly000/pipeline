#pragma once
#include "../UI/UIParameterAction.h"
#include "PipelineWrapper.h"

/*
 * MandelPipeline.h
 *
 *  Created on: May 31, 2016
 *      Author: tly
 */

EnumClass(MultisamplingPattern,
	"UNIFORM_GRID",
	"JITTERING"
);

EnumClass(CalculationMethod,
	"normal",
	"successive refinement",
	"Mariani-Silver",
	"grid",
	"successive iteration"
);

template<typename Factory,typename T> struct MandelPipeline : PipelineWrapper{
	UIParameterAction<
		KV("width",uint32_t),
		KV("height",uint32_t)
	> imageParams{"image"};

	UIParameterAction<
		KV("enable multisampling",bool),
		KV("size",uint32_t),
		KV("pattern",MultisamplingPattern)
	> multisampleParams{"multisampling"};

	UIParameterAction<
		KV("center real",T),
		KV("center imag",T),
		KV("scale",T),
		KV("angle",float)
	> positionParams{"position"};

	UIParameterAction<
		KV("formula",std::string),
		KV("enable juliamode",bool),
		KV("julia c real",T),
		KV("julia c imag",T),
		KV("calculation method",CalculationMethod)
	> calcParams{"calculation"};

	UIParameterAction<
		KV("iterations",uint32_t),
		KV("bailout",bool),
		KV("cycle detection",bool),
		KV("visualize cycle detection",bool)
	> algoParams{"escape time algorithm"};

	using FloatImage = typename Factory::template Image<float>;
	using ComplexImage = typename Factory::template Image<Vec<2,T>>;
	using Float3Image = typename Factory::template Image<Vec<3,float>>;
	using RGBAImage = typename Factory::template Image<uint32_t>;

	SlotAction<Input(
		KV("center real",T),
		KV("center imag",T),
		KV("scale",T),
		KV("angle",float),
		KV("positionImage",ComplexImage),
		KV("enable multisampling",bool),
		KV("pattern",MultisamplingPattern)
	),Output(
		KV("positionImage",ComplexImage)
	)> positionAction;

	SlotAction<Input(
		KV("formula",std::string),
		KV("enable juliamode",bool),
		KV("julia c real",T),
		KV("julia c imag",T),
		KV("iterations",uint32_t),
		KV("bailout",bool),
		KV("cycle detection",bool),
		KV("visualize cycle detection",bool),
		KV("positionImage",ComplexImage),
		KV("iterationImage",FloatImage)
	),Output(
		KV("iterationImage",ComplexImage)
	)> calcAction;

	SlotAction<Input(
		KV("iterationImage",FloatImage),
		KV("coloredImage",Float3Image)
	),Output(
		KV("coloredImage",Float3Image)
	)> coloringAction;

	SlotAction<Input(
		KV("enable multisampling",bool),
		KV("size",uint32_t),
		KV("coloredImage",Float3Image),
		KV("reducedImage",RGBAImage)
	),Output(
		KV("reducedImage",RGBAImage)
	)> reductionAction;

	//image generators
	FunctionCallAction<Input(
		KV("width",uint32_t),
		KV("height",uint32_t),
		KV("enable multisampling",bool),
		KV("size",uint32_t)
	),Output(
		KV("imageRange",Range)
	)> imageRangeGenerator{[](uint32_t w,uint32_t h,bool msEnabled,uint32_t s){
		return msEnabled ? Range{s * w, s * h} : Range{w,h};
	}};
	GeneratorAction<Input(
		KV("width",uint32_t),
		KV("height",uint32_t)
	),Output(
		KV("reducedImageRange",Range)
	)> reducedImageRangeGenerator;

	ImageGeneratorAction<Factory,Vec<2,T>> complexImageGenerator;
	ImageGeneratorAction<Factory,float> floatImageGenerator;
	ImageGeneratorAction<Factory,Vec<3,float>> float3ImageGenerator;
	ImageGeneratorAction<Factory,uint32_t> rgbaImageGenerator;

	MandelPipeline(Factory& factory):
	  complexImageGenerator(factory),
	  floatImageGenerator(factory),
	  float3ImageGenerator(factory),
	  rgbaImageGenerator(factory){
		this->addParam(imageParams);
		this->addParam(multisampleParams);
		this->addParam(positionParams);
		this->addParam(calcParams);
		this->addParam(algoParams);

		//connect positionaction
		positionParams.naturalConnect(positionAction);
		multisampleParams.naturalConnect(positionAction);

		imageRangeGenerator.naturalConnect(complexImageGenerator);
		complexImageGenerator.output<0>() >> positionAction.getInput("positionImage"_c);

		//connect calcAction
		calcParams.naturalConnect(calcAction);
		algoParams.naturalConnect(calcAction);
		positionAction.naturalConnect(calcAction);

		imageRangeGenerator.naturalConnect(floatImageGenerator);
		floatImageGenerator.output<0>() >> calcAction.getInput("iterationImage"_c);

		//connect coloringAction
		calcAction.naturalConnect(coloringAction);

		imageRangeGenerator.naturalConnect(float3ImageGenerator);
		float3ImageGenerator.output<0>() >> coloringAction.getInput("coloredImage"_c);

		//connect reductionAction
		multisampleParams.naturalConnect(reductionAction);
		coloringAction.naturalConnect(reductionAction);

		reducedImageRangeGenerator.naturalConnect(rgbaImageGenerator);
		rgbaImageGenerator.output<0>() >> reductionAction.getInput("reducedImage"_c);
	}
};


