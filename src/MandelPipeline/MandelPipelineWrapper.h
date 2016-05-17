#pragma once

#include "../Actions/GeneratorAction.h"
#include "../Actions/ImageGeneratorAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Type/Vec.h"
#include "../Type/Range.h"
#include "../UI/UIParameterAction.h"
#include "PipelineWrapper.h"
#include "../Type/TypeHelper.h"
#include "KernelDefinesAction.h"
#include "ToStringAction.h"
#include "../Actions/FunctionCallAction.h"
#include "ParserAction.h"

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
struct MandelPipelineWrapper : PipelineWrapper{
	using U32Image = typename Factory::template Image<uint32_t>;
	using FloatImage = typename Factory::template Image<float>;
	using ComplexImage = typename Factory::template Image<Vec<2,T>>;

	MandelPipelineWrapper(Factory& factory,const std::string& typeName);

	void run();

	const U32Image& getRenderedImage();

	UIParameterAction<T,T,T,float>& getPositionParam();

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
	UIParameterAction<std::string> generalParam{"general","formula"};

	//generates a Range object, fitting the sizeParam
	GeneratorAction<Input(uint32_t,uint32_t,uint32_t),Output(Range)> imageRangeGenerator;
	FunctionCallAction<Input(uint32_t,bool),Range> multisampleRangeGenerator{
		[](const uint32_t& multisampleSize,const bool& msEnabled)->Range{
 			return msEnabled ? Range{
				multisampleSize,multisampleSize,1
 			} : Range{1,1,1};
		}
	};
	FunctionCallAction<Input(Range,Range,bool),Range> multisampleSizeParam{
		[](const Range& imageRange,const Range& msRange, const bool& msEnabled)->Range{
 			return msEnabled ? Range{
				imageRange.x * msRange.x,
				imageRange.y * msRange.y,
				1
			} : imageRange;
		}
	};
	//generates defines used to compile the kernels
	KernelDefinesAction<7> kernelDefinesAction{
		"MAXITER","BAILOUT","Type",
		"MULTISAMPLING_ENABLED","MULTISAMPLING_SIZE","MULTISAMPLING_PATTERN",
		"FORMULA",
	};
	//generates strings for the defines
	ToStringAction<uint32_t,float,bool,uint32_t,uint32_t> toStringAction;
	PositionWrapper<Factory,T> position;
	CalculationWrapper<Factory,T> calculation;
	ColoringWrapper<Factory,T> coloring;
	ReductionWrapper<Factory,T> reduction;
	ParserAction formulaParser;
};

#include "PositionWrapper.h"
#include "CalculationWrapper.h"
#include "ReductionWrapper.h"
#include "ColoringWrapper.h"

#include "MandelPipelineWrapper.inl"
