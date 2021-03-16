#pragma once
#include "../Actions/BoxedAction.h"
#include "../Actions/BufferGeneratorAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelDefinesAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Actions/ImageGeneratorAction.h"
#include "../Type/Curve.h"
#include "../Type/Gradient.h"
#include "Enums.h"
#include "Typedefs.h"

/*
 * ColoringAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

struct ColoringAction :
	BoxedAction<Input(
        KV("numeric type", NumericType),
		KV("outer gradient",Gradient),
		KV("outer curve",Curve),
		KV("outer coloring method",std::string),
		KV("inner gradient",Gradient),
		KV("inner curve",Curve),
		KV("inner coloring method",std::string),
		KV("iterations",uint32_t),
		KV("bailout",float),
		KV("enable juliamode",bool),
		KV("iterationImage",FloatImage),
		KV("processedPositionImage",VariantComplexImage),
		KV("statsImage",Float4Image),
		KV("imageRange",Range)
	),Output(
		KV("coloredImage",Float3Image)
	)>{
	ColoringAction(Factory& factory);

	Factory& factory;

    ImageGeneratorAction<Vec<3,float>> coloredImageGenerator;

	FilledBufferGeneratorAction<Vec<3,float>>
		gradientBufferGenerator1,
		gradientBufferGenerator2;

	FilledBufferGeneratorAction<CurveSegment>
		curveBufferGenerator1,
		curveBufferGenerator2;

	KernelDefinesAction<
		KV("Type",NumericType),
		KV("MAXITER",uint32_t),
		KV("BAILOUT",float),
		KV("JULIAMODE",bool),
		KV("INSIDE_COLORING_KERNEL",std::string),
		KV("OUTSIDE_COLORING_KERNEL",std::string)
		> definesAction;
	KernelGeneratorAction<
		FloatImage,
        VariantComplexImage,
		Float4Image,
		Float3Image,
		Float3Buffer,
		Float3Buffer,
        Buffer<CurveSegment>,
        Buffer<CurveSegment>> kernelGeneratorAction;
	KernelAction<Input(
		KV("iterationImage",FloatImage),
		KV("processedPositionImage",VariantComplexImage),
		KV("statsImage",Float4Image),
		KV("coloredImage",Float3Image),
		KV("gradientA",Float3Buffer),
		KV("gradientB",Float3Buffer),
		KV("curveA",Buffer<CurveSegment>),
		KV("curveB",Buffer<CurveSegment>)
	), KernelOutput<3>> kernelAction;

protected:
	void executeImpl();
};
