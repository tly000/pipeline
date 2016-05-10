#pragma once
/*
 * PositionWrapper.h
 *
 *  Created on: May 10, 2016
 *      Author: tly
 */

template<typename Factory,typename T>
struct PositionWrapper : NonCopyable{
	using FloatImage = typename Factory::template Image<float>;
	using ComplexImage = typename Factory::template Image<Vec<2,T>>;

	PositionWrapper(MandelPipelineWrapper<Factory,T>* pipeline)
		:positionImageGenerator(pipeline->factory),
		 positionKernelGenerator(pipeline->factory){
		pipeline->multisampleSizeParam.output(0) >> positionImageGenerator.input(0);

		positionKernelGenerator.template getInput<0>().setDefaultValue("position");
		positionKernelGenerator.template getInput<1>().setDefaultValue("positionKernel");
		pipeline->kernelDefinesAction.output(0) >> positionKernelGenerator.input(2);
		positionKernelGenerator.output(0) >> positionKernel.getKernelInput();

		positionImageGenerator.output(0) >> positionKernel.kernelInput(0);
		pipeline->imageRangeGenerator.output(0) >> positionKernel.getGlobalSizeInput();
		pipeline->multisampleRangeGenerator.output(0) >> positionKernel.getLocalSizeInput();

		positionParam.output(0,1,2) >> positionKernel.kernelInput(1,2,3);
	}

	UIParameterAction<T,T,T> positionParam{"position","real position","imag position","scale"};
	//generates an image of size "multisampleSizeParam"
	ImageGeneratorAction<Factory,Vec<2,T>> positionImageGenerator;
	KernelGeneratorAction<Factory,ComplexImage,T,T,T> positionKernelGenerator;
	//the position kernel
	//input: ComplexImage to be filled with position data, T offsetReal, T offsetImage, T scale
	//output: input 0 (the filled image)
	KernelAction<
		Factory,
		Input(ComplexImage positionImage,T,T,T),
		KernelOutput<0>
	> positionKernel;
};




