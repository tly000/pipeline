#pragma once
#include "WrappedMandelPipeline.h"

/*
 * CalculationWrapper.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

template<typename Factory,typename T>
struct CalculationWrapper : NonCopyable{
	using FloatImage = typename Factory::template Image<float>;
	using ComplexImage = typename Factory::template Image<Vec<2,T>>;

	CalculationWrapper(WrappedMandelPipeline<Factory,T>* pipeline){
		iterationParam.setValue<0>(64);//iterations
		iterationParam.setValue<1>(4);//bailout²
		//calculation
		pipeline->multisampleSizeParam.output(0) >> mandelbrotImageGenerator.input(0);

		mandelbrotKernelGenerator.template getInput<0>().setDefaultValue("calculation");
		mandelbrotKernelGenerator.template getInput<1>().setDefaultValue("mandelbrotKernel");
		pipeline->kernelDefinesAction.output(0) >> mandelbrotKernelGenerator.input(2);
		mandelbrotKernelGenerator.output(0) >> mandelbrotKernel.getKernelInput();

		pipeline->positionKernel.output(0) >> mandelbrotKernel.kernelInput(0);
		mandelbrotImageGenerator.output(0) >> mandelbrotKernel.kernelInput(1);
		pipeline->imageRangeGenerator.output(0) >> mandelbrotKernel.getGlobalSizeInput();
		pipeline->multisampleRangeGenerator.output(0) >> mandelbrotKernel.getLocalSizeInput();
	}

	UIParameterAction<unsigned,float> iterationParam{"iteration", "iterations", "bailout"};
	//generates an image of size "sizeParam"
	ImageGeneratorAction<Factory,float> mandelbrotImageGenerator;
	KernelGeneratorAction<Factory,ComplexImage,FloatImage> mandelbrotKernelGenerator;
	//the main mandelbrot calculation
	//input: ComplexImage holding complex coordinates, FloatImage to save the calculated iteration count
	//output: input 1 (the iteration count image)
	KernelAction<
		Factory,
		Input(ComplexImage positions,FloatImage iterationOutput),
		KernelOutput<1>
	> mandelbrotKernel;

};


