#pragma once
#include "WrappedMandelPipeline.h"

/*
 * ColoringWrapper.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

template<typename Factory,typename T>
struct ColoringWrapper : NonCopyable{
	using FloatImage = typename Factory::template Image<float>;
	using U32Image = typename Factory::template Image<uint32_t>;

	ColoringWrapper(WrappedMandelPipeline<Factory,T>* pipeline){
		pipeline->imageRangeGenerator.output(0) >> colorImageGenerator.input(0);

		coloringKernelGenerator.template getInput<0>().setDefaultValue("coloring");
		coloringKernelGenerator.template getInput<1>().setDefaultValue("coloringKernel");
		pipeline->kernelDefinesAction.output(0) >> coloringKernelGenerator.input(2);
		coloringKernelGenerator.output(0) >> coloringKernel.getKernelInput();

		pipeline->reductionKernel.output(0) >> coloringKernel.kernelInput(0);
		colorImageGenerator.output(0) >> coloringKernel.kernelInput(1);
		pipeline->imageRangeGenerator.output(0) >> coloringKernel.getGlobalSizeInput();
	}

	//coloring kernel
	//generates an image of size "sizeParam" holding RGBA values
	ImageGeneratorAction<Factory,unsigned> colorImageGenerator;
	KernelGeneratorAction<Factory,FloatImage,U32Image> coloringKernelGenerator;
	//the coloring kernel
	//input: FloatImage holding the iteration count, U32Image to write the color data
	//output: input 1 (the colored image)
	KernelAction<
		Factory,
		Input(FloatImage iterationInput,U32Image colorOutput),
		KernelOutput<1>
	> coloringKernel;
};



