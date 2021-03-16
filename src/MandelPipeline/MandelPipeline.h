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
#include "../Platform/Factory.h"

#include "Typedefs.h"
#include "Enums.h"
#include "VariantUtils.h"
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

struct MandelPipeline : PipelineWrapper{
    std::shared_ptr<Factory> factory;

    UIParameterAction<
        KV("numeric type",NumericType)
    > typeParam{"type"};

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
		KV("center real",HighPrecisionType),
		KV("center imag",HighPrecisionType),
		KV("scale",HighPrecisionType),
		KV("angle",float)
	> positionParams{"position"};

    NumberToVariantConversionAction<typename decltype(positionParams)::ThisType> convertedPositionParams;

	UIParameterAction<
        KV("formula",std::string),
		KV("enable juliamode",bool),
		KV("julia c real",HighPrecisionType),
		KV("julia c imag",HighPrecisionType),
		KV("calculation method",CalculationMethod),
		KV("visualize steps",bool)
	> calcParams{"calculation"};

    NumberToVariantConversionAction<typename decltype(calcParams)::ThisType> convertedCalcParams;

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

	PositionAction positionAction;

	SlotAction<Input(
        KV("numeric type", NumericType),
		KV("visualize steps",bool),
		KV("reset calculation",bool),
		KV("processed formula",std::string),
		KV("enable juliamode",bool),
		KV("julia c real",VariantNumericType),
		KV("julia c imag",VariantNumericType),
		KV("iterations",uint32_t),
		KV("bailout",float),
		KV("disable bailout",bool),
		KV("cycle detection",bool),
		KV("visualize cycle detection",bool),
		KV("smooth iteration count",bool),
		KV("leading polynomial exponent",float),
		KV("statistic function",std::string),
		KV("positionImage",VariantComplexImage),
		KV("imageRange",Range)
	),Output(
		KV("iterationImage",FloatImage),
		KV("processedPositionImage",VariantComplexImage),
		KV("statsImage",Float4Image),
		KV("time",uint64_t),
		KV("done",bool)
	)> calcAction;

	using CalcActionType = typename decltype(calcAction)::SlotActionType;

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

    ColoringAction coloringAction;
    ReductionAction reductionAction;

	ParserAction parserAction;

	std::map<std::string,std::unique_ptr<CalcActionType>> methodMap;
	std::map<std::string,std::function<std::unique_ptr<CalcActionType>()>> creationMap;
	FunctionCallAction<Input(
		KV("calculation method",CalculationMethod)
	),
		KV("action",CalcActionType*)
	> methodSelectionAction;

	MandelPipeline(std::shared_ptr<Factory> factory);

	void run();
};


