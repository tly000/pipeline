#pragma once
#include "../Type/StringConstant.h"
#include "../Type/EnumClass.h"
#include "../UI/UIParameterAction.h"
#include "PipelineWrapper.h"
#include "../Actions/SlotAction.h"
#include "../Actions/FunctionCallAction.h"
#include "../Actions/GeneratorAction.h"
#include "../Actions/ImageGeneratorAction.h"
#include "../Utility/Timer.h"
#include "CalculationActionBase.h"

#include "Enums.h"
#include "ParserAction.h"

#include "PositionAction.h"
#include "ColoringAction.h"
#include "ReductionAction.h"

#include "NormalCalculationAction.h"
#include "GridbasedCalculationAction.h"
#include "SuccessiveRefinmentAction.h"

/*
 * MandelPipeline.h
 *
 *  Created on: May 31, 2016
 *      Author: tly
 */

template<typename Factory,typename T> struct MandelPipeline : PipelineWrapper{
	Factory factory;
	std::string typeName;

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
		KV("calculation method",CalculationMethod),
		KV("visualize steps",bool)
	> calcParams{"calculation"};

	UIParameterAction<
		KV("iterations",uint32_t),
		KV("bailout",float),
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
		KV("pattern",MultisamplingPattern),
		KV("imageRange",Range)
	),Output(
		KV("positionImage",ComplexImage)
	)> positionAction;

	SlotAction<Input(
		KV("visualize steps",bool),
		KV("reset calculation",bool),
		KV("processed formula",std::string),
		KV("enable juliamode",bool),
		KV("julia c real",T),
		KV("julia c imag",T),
		KV("iterations",uint32_t),
		KV("bailout",float),
		KV("cycle detection",bool),
		KV("visualize cycle detection",bool),
		KV("positionImage",ComplexImage),
		KV("iterationImage",FloatImage),
		KV("imageRange",Range)
	),Output(
		KV("iterationImage",FloatImage),
		KV("done",bool)
	)> calcAction;

	using CalcActionType = typename decltype(calcAction)::SlotActionType;

	SlotAction<Input(
		KV("iterations",uint32_t),
		KV("iterationImage",FloatImage),
		KV("coloredImage",Float3Image),
		KV("imageRange",Range)
	),Output(
		KV("coloredImage",Float3Image)
	)> coloringAction;

	SlotAction<Input(
		KV("enable multisampling",bool),
		KV("size",uint32_t),
		KV("coloredImage",Float3Image),
		KV("reducedImage",RGBAImage),
		KV("imageRange",Range)
	),Output(
		KV("reducedImage",RGBAImage)
	)> reductionAction;

	//image generators
	FunctionCallAction<Input(
		KV("width",uint32_t),
		KV("height",uint32_t),
		KV("enable multisampling",bool),
		KV("size",uint32_t)
	),
		KV("imageRange",Range)
	> imageRangeGenerator{[](uint32_t w,uint32_t h,bool msEnabled,uint32_t s){
		return msEnabled ? Range{s * w, s * h,1} : Range{w,h,1};
	}};
	GeneratorAction<Input(
		KV("width",uint32_t),
		KV("height",uint32_t)
	),Output(
		KV("imageRange",Range)
	)> reducedImageRangeGenerator;

	ImageGeneratorAction<Factory,Vec<2,T>> complexImageGenerator;
	ImageGeneratorAction<Factory,float> floatImageGenerator;
	ImageGeneratorAction<Factory,Vec<3,float>> float3ImageGenerator;
	ImageGeneratorAction<Factory,uint32_t> rgbaImageGenerator;

	ParserAction parserAction;
	PositionAction<Factory,T> positionActionImpl;
	ColoringAction<Factory,T> coloringActionImpl;
	ReductionAction<Factory> reductionActionImpl;

	std::map<std::string,std::unique_ptr<CalcActionType>> methodMap;
	std::map<std::string,std::function<std::unique_ptr<CalcActionType>()>> creationMap{
		{ "normal", [this]{
			return std::make_unique<NormalCalculationAction<Factory,T>>(factory,typeName);
		}},
		{ "grid", [this]{
			return std::make_unique<GridbasedCalculationAction<Factory,T>>(factory,typeName);
		}},
		{ "successive refinement", [this]{
			return std::make_unique<SuccessiveRefinementAction<Factory,T>>(factory,typeName);
		}},
		{ "Mariani-Silver", [this]()->std::unique_ptr<CalcActionType>{
			throw std::runtime_error("Mariani-Silver not implemented.");
		}},
		{ "successive iteration", [this]()->std::unique_ptr<CalcActionType>{
			throw std::runtime_error("successive iteration not implemented.");
		}},
	};
	FunctionCallAction<Input(
		KV("calculation method",CalculationMethod)
	),
		KV("action",CalcActionType*)
	> methodSelectionAction{
		[this](const CalculationMethod& method){
			std::string s = method.getString();
			if(!methodMap.count(s)){
				methodMap.emplace(s,creationMap.at(s)());
			}
			return methodMap.at(method.getString()).get();
		}
	};

	MandelPipeline(Factory& factory,std::string typeName):
	  factory(factory),
	  typeName(typeName),
	  complexImageGenerator(factory),
	  floatImageGenerator(factory),
	  float3ImageGenerator(factory),
	  rgbaImageGenerator(factory),
	  positionActionImpl(factory,typeName),
	  coloringActionImpl(factory,typeName),
	  reductionActionImpl(factory,typeName)
	{
		this->addParam(imageParams);
		this->addParam(multisampleParams);
		this->addParam(positionParams);
		this->addParam(calcParams);
		this->addParam(algoParams);

		imageParams.naturalConnect(imageRangeGenerator);
		multisampleParams.naturalConnect(imageRangeGenerator);
		imageParams.naturalConnect(reducedImageRangeGenerator);

		//connect positionaction
		positionParams.naturalConnect(positionAction);
		multisampleParams.naturalConnect(positionAction);

		imageRangeGenerator.naturalConnect(positionAction);
		imageRangeGenerator.naturalConnect(complexImageGenerator);
		complexImageGenerator.template output<0>() >> positionAction.getInput("positionImage"_c);
		positionAction.getActionInput().setDefaultValue(&positionActionImpl);

		//connect calcAction
		calcParams.naturalConnect(parserAction);
		parserAction.naturalConnect(calcAction);

		calcParams.naturalConnect(calcAction);
		algoParams.naturalConnect(calcAction);
		positionAction.naturalConnect(calcAction);

		imageRangeGenerator.naturalConnect(calcAction);
		imageRangeGenerator.naturalConnect(floatImageGenerator);
		floatImageGenerator.template output<0>() >> calcAction.getInput("iterationImage"_c);

		calcParams.naturalConnect(methodSelectionAction);
		methodSelectionAction.naturalConnect(calcAction);
		calcAction.getInput("reset calculation"_c).setDefaultValue(true);

		//connect coloringAction
		algoParams.naturalConnect(coloringAction);
		calcAction.naturalConnect(coloringAction);

		imageRangeGenerator.naturalConnect(coloringAction);
		imageRangeGenerator.naturalConnect(float3ImageGenerator);
		float3ImageGenerator.template output<0>() >> coloringAction.getInput("coloredImage"_c);
		coloringAction.getActionInput().setDefaultValue(&coloringActionImpl);

		//connect reductionAction
		multisampleParams.naturalConnect(reductionAction);
		coloringAction.naturalConnect(reductionAction);

		reducedImageRangeGenerator.naturalConnect(reductionAction);
		reducedImageRangeGenerator.naturalConnect(rgbaImageGenerator);
		rgbaImageGenerator.template output<0>() >> reductionAction.getInput("reducedImage"_c);
		reductionAction.getActionInput().setDefaultValue(&reductionActionImpl);

		//set default values:
		imageParams.setValue("width"_c,500);
		imageParams.setValue("height"_c,500);

		multisampleParams.setValue("enable multisampling"_c,false);
		multisampleParams.setValue("size"_c,2);
		multisampleParams.setValue("pattern"_c,"UNIFORM_GRID");

		positionParams.setValue("center real"_c,fromString<T>("-0.5"));
		positionParams.setValue("center imag"_c,fromString<T>("0"));
		positionParams.setValue("scale"_c,fromString<T>("2"));
		positionParams.setValue("angle"_c,0);

		calcParams.setValue("formula"_c,"z*z + c");
		calcParams.setValue("enable juliamode"_c,false);
		calcParams.setValue("julia c real"_c,fromString<T>("0"));
		calcParams.setValue("julia c imag"_c,fromString<T>("0"));
		calcParams.setValue("calculation method"_c,"normal");
		calcParams.setValue("visualize steps"_c,false);

		algoParams.setValue("iterations"_c,64);
		algoParams.setValue("bailout"_c,4);
		algoParams.setValue("cycle detection"_c,false);
		algoParams.setValue("visualize cycle detection"_c,false);
	}

	void run(){
		_logDebug("[info] running pipeline " + demangle(typeid(*this)));
		Timer timer;
		timer.start();
		reductionAction.run();
		auto fullTime = timer.stop();
		_log("[info] full: " << fullTime << " us.");
	}
};


