#pragma once
#include "../Actions/BoxedAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Actions/KernelDefinesAction.h"
#include "../Actions/FunctionCallAction.h"
#include "../Actions/BufferGeneratorAction.h"
#include "../Type/Vec.h"
#include "Enums.h"
#include "Typedefs.h"

/*
 * PositionAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

struct PositionAction :
	BoxedAction<Input(
        KV("platform", std::shared_ptr<Factory>),
        KV("numeric type", NumericType),
		KV("center real",VariantNumericType),
		KV("center imag",VariantNumericType),
		KV("scale",VariantNumericType),
		KV("angle",float),
		KV("enable multisampling",bool),
		KV("pattern",MultisamplingPattern),
		KV("imageRange",Range)
	),Output(
		KV("positionImage",VariantComplexImage)
	)>{
	PositionAction();

    FunctionCallAction<Input(
        KV("platform", std::shared_ptr<Factory>),
        KV("numeric type",NumericType),
        KV("imageRange",Range)
    ), KV("positionImage",VariantComplexImage)> positionImageGenerator;
	KernelDefinesAction<KV("Type",NumericType),KV("MULTISAMPLING_ENABLED",bool),KV("MULTISAMPLING_PATTERN",MultisamplingPattern)> definesAction;
	KernelGeneratorAction<VariantComplexImage,VariantNumericType,VariantNumericType,VariantVec2> kernelGeneratorAction;
	KernelAction<Input(
		KV("positionImage",VariantComplexImage),
		KV("center real",VariantNumericType),
		KV("center imag",VariantNumericType),
		KV("rotation",VariantVec2)
	), KernelOutput<0>> kernelAction;

	FunctionCallAction<Input(
		KV("scale",VariantNumericType),
		KV("angle",float)
	), KV("rotation",VariantVec2)> rotationAction;

protected:
	void executeImpl();
};
