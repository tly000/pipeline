#pragma once
#include "MandelPipelineWrapper.h"

/*
 * ColoringWrapper.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

template<typename Factory,typename T>
struct ColoringWrapper : NonCopyable{
	using FloatImage = typename Factory::template Image<float>;
	using Vec3Image = typename Factory::template Image<Vec<3,float>>;

	ColoringWrapper(MandelPipelineWrapper<Factory,T>* pipeline)
		:colorImageGenerator(pipeline->factory),
		 coloringKernelGenerator(pipeline->factory){
		pipeline->multisampleSizeParam.output(0) >> colorImageGenerator.input(0);

		coloringKernelGenerator.template getInput<0>().setDefaultValue("coloring");
		coloringKernelGenerator.template getInput<1>().setDefaultValue("coloringKernel");
		pipeline->kernelDefinesAction.output(0) >> coloringKernelGenerator.input(2);
		coloringKernelGenerator.output(0) >> coloringKernel.getKernelInput();

		pipeline->calculation.mandelbrotKernel.output(0) >> coloringKernel.kernelInput(0);
		colorImageGenerator.output(0) >> coloringKernel.kernelInput(1);
		pipeline->multisampleSizeParam.output(0) >> coloringKernel.getGlobalSizeInput();
	}

	//coloring kernel
	//generates an image of size "sizeParam" holding RGBA values
	ImageGeneratorAction<Factory,Vec<3,float>> colorImageGenerator;
	KernelGeneratorAction<Factory,FloatImage,Vec3Image> coloringKernelGenerator;
	//the coloring kernel
	//input: FloatImage holding the iteration count, U32Image to write the color data
	//output: input 1 (the colored image)
	KernelAction<
		Factory,
		Input(FloatImage iterationInput,Vec3Image colorOutput),
		KernelOutput<1>
	> coloringKernel;
};



