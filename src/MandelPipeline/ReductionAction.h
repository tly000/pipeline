#pragma once
#include "../Actions/BoxedAction.h"
#include "../Type/Vec.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Actions/KernelDefinesAction.h"
#include "../Actions/ImageGeneratorAction.h"
#include "Enums.h"
#include "Typedefs.h"

/*
 * ReductionAction.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

struct ReductionAction : BoxedAction<Input(
    KV("numeric type", NumericType),
	KV("enable multisampling",bool),
	KV("size",uint32_t),
	KV("coloredImage",Float3Image),
	KV("imageRange",Range)
),Output(
	KV("reducedImage",RGBAImage)
)>{
	ReductionAction(Factory& factory);

    ImageGeneratorAction<std::uint32_t> reducedImageGenerator;
	KernelDefinesAction<
	    KV("Type",NumericType),
        KV("MULTISAMPLING_ENABLED",bool),
        KV("MULTISAMPLING_SIZE",uint32_t)> definesAction;
	KernelGeneratorAction<Float3Image,RGBAImage> kernelGeneratorAction;
	KernelAction<Input(
		KV("coloredImage",Float3Image),
		KV("reducedImage",RGBAImage)
	), KernelOutput<1>> kernelAction;

protected:
	void executeImpl();
};

