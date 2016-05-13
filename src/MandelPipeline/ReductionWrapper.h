#pragma once
#include "MandelPipelineWrapper.h"

/*
 * ReductionWrapper.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

template<typename Factory,typename T>
struct ReductionWrapper : NonCopyable{
	using Vec3Image = typename Factory::template Image<Vec<3,float>>;
	using U32Image = typename Factory::template Image<unsigned>;

	ReductionWrapper(MandelPipelineWrapper<Factory,T>* pipeline)
		:reducedColorImageGenerator(pipeline->factory),
		 reductionKernelGenerator(pipeline->factory){
		//reduction
		pipeline->imageRangeGenerator.output(0) >> reducedColorImageGenerator.input(0);

		reductionKernelGenerator.template getInput<0>().setDefaultValue("reduction");
		reductionKernelGenerator.template getInput<1>().setDefaultValue("reductionKernel");
		pipeline->kernelDefinesAction.output(0) >> reductionKernelGenerator.input(2);
		reductionKernelGenerator.output(0) >> reductionKernel.getKernelInput();

		pipeline->coloring.coloringKernel.output(0) >> reductionKernel.kernelInput(0);
		reducedColorImageGenerator.output(0) >> reductionKernel.kernelInput(1);
		pipeline->imageRangeGenerator.output(0) >> reductionKernel.getGlobalSizeInput();
	}

	ImageGeneratorAction<Factory,uint32_t> reducedColorImageGenerator;
	KernelGeneratorAction<Factory,Vec3Image,U32Image> reductionKernelGenerator;
	//the reduction kernel: reduces the multisampled image
	//input: Vec3Image holding the colored image,
	//output: reduced U32Image holding the colors
	KernelAction<
		Factory,
		Input(Vec3Image colorImageInput,U32Image colorImageOutput),
		KernelOutput<1>
	> reductionKernel;
};


