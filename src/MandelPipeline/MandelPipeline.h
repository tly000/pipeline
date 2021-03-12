#pragma once
#include "../Type/StringConstant.h"
#include "../Type/EnumClass.h"
#include "../Type/Gradient.h"
#include "../UI/UIParameterAction.h"
#include "PipelineWrapper.h"
#include "../Actions/SlotAction.h"
#include "../Actions/FunctionCallAction.h"
#include "../Actions/GeneratorAction.h"
#include "../Actions/ImageGeneratorAction.h"
#include "../Utility/Timer.h"
#include "CalculationActionBase.h"
#include "../Type/Curve.h"

#include "Enums.h"
#include "ParserAction.h"

#include "PositionAction.h"
#include "ColoringAction.h"
#include "ReductionAction.h"

#include "NormalCalculationAction.h"
#include "GridbasedCalculationAction.h"
#include "SuccessiveRefinementAction.h"
#include "SuccessiveIterationAction.h"
#include "MarianiSilverAction.h"

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
		KV("disable bailout",bool),
		KV("cycle detection",bool),
		KV("visualize cycle detection",bool),
		KV("smooth iteration count",bool),
		KV("leading polynomial exponent",float),
		KV("statistic function",std::string)
	> algoParams{"escape time algorithm"};

	UIParameterAction<
		KV("outer gradient",Gradient),
		KV("outer curve",Curve),
		KV("outer coloring method",std::string),
		KV("inner gradient",Gradient),
		KV("inner curve",Curve),
		KV("inner coloring method",std::string)
	> colorParams{"coloring"};

	SlotAction<Input(
		KV("center real",T),
		KV("center imag",T),
		KV("scale",T),
		KV("angle",float),
		KV("positionImage",ComplexImage<Factory,T>),
		KV("enable multisampling",bool),
		KV("pattern",MultisamplingPattern),
		KV("imageRange",Range)
	),Output(
		KV("positionImage",ComplexImage<Factory,T>)
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
		KV("disable bailout",bool),
		KV("cycle detection",bool),
		KV("visualize cycle detection",bool),
		KV("smooth iteration count",bool),
		KV("leading polynomial exponent",float),
		KV("statistic function",std::string),
		KV("positionImage",ComplexImage<Factory,T>),
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("statsImage",Float4Image<Factory>),
		KV("imageRange",Range)
	),Output(
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("statsImage",Float4Image<Factory>),
		KV("time",uint64_t),
		KV("done",bool)
	)> calcAction;

	using CalcActionType = typename decltype(calcAction)::SlotActionType;

	SlotAction<Input(
		KV("outer gradient",Gradient),
		KV("outer curve",Curve),
		KV("outer coloring method",std::string),
		KV("inner gradient",Gradient),
		KV("inner curve",Curve),
		KV("inner coloring method",std::string),
		KV("iterations",uint32_t),
		KV("bailout",float),
		KV("enable juliamode",bool),
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("statsImage",Float4Image<Factory>),
		KV("coloredImage",Float3Image<Factory>),
		KV("imageRange",Range)
	),Output(
		KV("coloredImage",Float3Image<Factory>)
	)> coloringAction;

	SlotAction<Input(
		KV("enable multisampling",bool),
		KV("size",uint32_t),
		KV("coloredImage",Float3Image<Factory>),
		KV("reducedImage",RGBAImage<Factory>),
		KV("imageRange",Range)
	),Output(
		KV("reducedImage",RGBAImage<Factory>)
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

	ImageGeneratorAction<Factory,Vec<2,T>> complexImageGeneratorA;
	ImageGeneratorAction<Factory,Vec<2,T>> complexImageGeneratorB;
	ImageGeneratorAction<Factory,float> floatImageGenerator;
	ImageGeneratorAction<Factory,Vec<3,float>> float3ImageGenerator;
	ImageGeneratorAction<Factory,Vec<4,float>> float4ImageGenerator;
	ImageGeneratorAction<Factory,uint32_t> rgbaImageGenerator;

	ParserAction parserAction;
	PositionAction<Factory,T> positionActionImpl;
	ColoringAction<Factory,T> coloringActionImpl;
	ReductionAction<Factory> reductionActionImpl;

	std::map<std::string,std::unique_ptr<CalcActionType>> methodMap;
	std::map<std::string,std::function<std::unique_ptr<CalcActionType>()>> creationMap;
	FunctionCallAction<Input(
		KV("calculation method",CalculationMethod)
	),
		KV("action",CalcActionType*)
	> methodSelectionAction;

	MandelPipeline(Factory& factory,std::string typeName):
	  factory(factory),
	  typeName(typeName),
	  complexImageGeneratorA{factory},
	  complexImageGeneratorB{factory},
	  floatImageGenerator(factory),
	  float3ImageGenerator(factory),
	  float4ImageGenerator(factory),
	  rgbaImageGenerator(factory),
	  positionActionImpl(factory,typeName),
	  coloringActionImpl(factory,typeName),
	  reductionActionImpl(factory,typeName),
	  creationMap{
		  { "normal", [this]{
			  return std::make_unique<NormalCalculationAction<Factory,T>>(this->factory,this->typeName);
		  }},
		  { "grid", [this]{
			  return std::make_unique<GridbasedCalculationAction<Factory,T>>(this->factory,this->typeName);
		  }},
		  { "successive refinement", [this]{
			  return std::make_unique<SuccessiveRefinementAction<Factory,T>>(this->factory,this->typeName);
		  }},
		  { "Mariani-Silver", [this]{
			  return std::make_unique<MarianiSilverAction<Factory,T>>(this->factory,this->typeName);
		  }},
		  { "successive iteration", [this]{
			  return std::make_unique<SuccessiveIterationAction<Factory,T>>(this->factory,this->typeName);
		  }},
	  },
	  methodSelectionAction{
		  [this](const CalculationMethod& method){
			  std::string s = method.getString();
			  if(!methodMap.count(s)){
				  methodMap.emplace(s,creationMap.at(s)());
			  }
			  return methodMap.at(method.getString()).get();
		  }
	  }
	{
		this->addParam(imageParams);
		this->addParam(multisampleParams);
		this->addParam(positionParams);
		this->addParam(calcParams);
		this->addParam(algoParams);
		this->addParam(colorParams);

		imageParams.naturalConnect(imageRangeGenerator);
		multisampleParams.naturalConnect(imageRangeGenerator);
		imageParams.naturalConnect(reducedImageRangeGenerator);

		//connect positionaction
		positionParams.naturalConnect(positionAction);
		multisampleParams.naturalConnect(positionAction);

		imageRangeGenerator.naturalConnect(positionAction);
		imageRangeGenerator.naturalConnect(complexImageGeneratorA);
		imageRangeGenerator.naturalConnect(complexImageGeneratorB);

		complexImageGeneratorA.template output<0>() >> positionAction.getInput(_C("positionImage"));
		positionAction.getActionInput().setDefaultValue(&positionActionImpl);

		//connect calcAction
		calcParams.naturalConnect(parserAction);
		parserAction.naturalConnect(calcAction);

		calcParams.naturalConnect(calcAction);
		algoParams.naturalConnect(calcAction);
		positionAction.naturalConnect(calcAction);

		imageRangeGenerator.naturalConnect(calcAction);
		imageRangeGenerator.naturalConnect(floatImageGenerator);
		imageRangeGenerator.naturalConnect(float4ImageGenerator);
		floatImageGenerator.template output<0>() >> calcAction.getInput(_C("iterationImage"));
		float4ImageGenerator.template output<0>() >> calcAction.getInput(_C("statsImage"));
		complexImageGeneratorB.template output<0>() >> calcAction.getInput(_C("processedPositionImage"));

		calcParams.naturalConnect(methodSelectionAction);
		methodSelectionAction.naturalConnect(calcAction);
		calcAction.getInput(_C("reset calculation")).setDefaultValue(true);

		//connect coloringAction
		algoParams.naturalConnect(coloringAction);
		calcParams.naturalConnect(coloringAction);
		calcAction.naturalConnect(coloringAction);
		colorParams.naturalConnect(coloringAction);

		imageRangeGenerator.naturalConnect(coloringAction);
		imageRangeGenerator.naturalConnect(float3ImageGenerator);
		float3ImageGenerator.template output<0>() >> coloringAction.getInput(_C("coloredImage"));
		coloringAction.getActionInput().setDefaultValue(&coloringActionImpl);

		//connect reductionAction
		multisampleParams.naturalConnect(reductionAction);
		coloringAction.naturalConnect(reductionAction);

		reducedImageRangeGenerator.naturalConnect(reductionAction);
		reducedImageRangeGenerator.naturalConnect(rgbaImageGenerator);
		rgbaImageGenerator.template output<0>() >> reductionAction.getInput(_C("reducedImage"));
		reductionAction.getActionInput().setDefaultValue(&reductionActionImpl);

		//set default values:
		imageParams.setValue(_C("width"),512);
		imageParams.setValue(_C("height"),512);

		multisampleParams.setValue(_C("enable multisampling"),false);
		multisampleParams.setValue(_C("size"),2);
		multisampleParams.setValue(_C("pattern"),"UNIFORM_GRID");

		positionParams.setValue(_C("center real"),fromString<T>("-0.5"));
		positionParams.setValue(_C("center imag"),fromString<T>("0"));
		positionParams.setValue(_C("scale"),fromString<T>("2"));
		positionParams.setValue(_C("angle"),0);

		calcParams.setValue(_C("formula"),"z*z + c");
		calcParams.setValue(_C("enable juliamode"),false);
		calcParams.setValue(_C("julia c real"),fromString<T>("0"));
		calcParams.setValue(_C("julia c imag"),fromString<T>("0"));
		calcParams.setValue(_C("calculation method"),"normal");
		calcParams.setValue(_C("visualize steps"),false);

		algoParams.setValue(_C("iterations"),64);
		algoParams.setValue(_C("bailout"),16);
		algoParams.setValue(_C("disable bailout"),false);
		algoParams.setValue(_C("cycle detection"),false);
		algoParams.setValue(_C("smooth iteration count"),false);
		algoParams.setValue(_C("leading polynomial exponent"),2);
		algoParams.setValue(_C("visualize cycle detection"),false);
		algoParams.setValue(_C("statistic function"),"noStats");

		colorParams.setValue(_C("outer gradient"),Gradient{{0.0f,0.0f,0.0f},{0.0f,0.7f,1.0f}});
		colorParams.setValue(_C("outer curve"),Curve{CurveSegment{
			{0,0},{1,1},1,1
		}});
		colorParams.setValue(_C("outer coloring method"),"iterationGradient");
		colorParams.setValue(_C("inner gradient"),Gradient{{0.0f,0.0f,0.0f}});
		colorParams.setValue(_C("inner curve"),Curve{CurveSegment{
			{0,0},{1,1},1,1
		}});
		colorParams.setValue(_C("inner coloring method"),"flatColor");
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


