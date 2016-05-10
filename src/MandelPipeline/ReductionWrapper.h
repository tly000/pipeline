#pragma once
#include "WrappedMandelPipeline.h"

/*
 * ReductionWrapper.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

template<typename Factory,typename T>
struct ReductionWrapper : NonCopyable{
	using FloatImage = typename Factory::template Image<float>;

	ReductionWrapper(WrappedMandelPipeline<Factory,T>* pipeline){
		//reduction
		pipeline->imageRangeGenerator.output(0) >> reducedIterationImageGenerator.input(0);

		reductionKernelGenerator.template getInput<0>().setDefaultValue("reduction");
		reductionKernelGenerator.template getInput<1>().setDefaultValue("reductionKernel");
		pipeline->kernelDefinesAction.output(0) >> reductionKernelGenerator.input(2);
		reductionKernelGenerator.output(0) >> reductionKernel.getKernelInput();

		pipeline->mandelbrotKernel.output(0) >> reductionKernel.kernelInput(0);
		reducedIterationImageGenerator.output(0) >> reductionKernel.kernelInput(1);
		pipeline->imageRangeGenerator.output(0) >> reductionKernel.getGlobalSizeInput();
		pipeline->multisampleRangeGenerator.output(0) >> reductionKernel.getLocalSizeInput();
	}

	//generates an image of size "multisampleSizeParam"
	ImageGeneratorAction<Factory,float> reducedIterationImageGenerator;
	KernelGeneratorAction<Factory,FloatImage,FloatImage> reductionKernelGenerator;
	//the reduction kernel: reduces the multisampled image
	//input: FloatImage holding the iteration count,
	//output: reduced FloatImage holding the iteration count
	KernelAction<
		Factory,
		Input(FloatImage iterationInput,FloatImage iterationOutput),
		KernelOutput<1>
	> reductionKernel;
};


