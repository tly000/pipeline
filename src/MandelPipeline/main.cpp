#include "../Actions/ParameterAction.h"
#include "../Platform/CPU/CPUFactory.h"
#include "ImageGeneratorAction.h"
#include "KernelGeneratorAction.h"
#include "KernelAction.h"
#include "../Actions/GeneratorAction.h"
#include <BackTracer.h>

/*
 * main.cpp
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

int main(){
	BackTracer::registerErrorHandler();
	CPUFactory factory;

	ParameterAction<uint32_t,uint32_t> sizeParam;
	sizeParam.setValue<0>(512);
	sizeParam.setValue<1>(512);

	GeneratorAction<Input(uint32_t,uint32_t,uint32_t),Output(Range)> imageRangeGenerator;
	sizeParam.output(0,1) >> imageRangeGenerator.input(0,1);
	imageRangeGenerator.getInput<2>().setDefaultValue(1);

	//position
		ImageGeneratorAction<CPUFactory,Complex> positionImageGenerator(factory);
		sizeParam.output(0,1) >> positionImageGenerator.input(0,1);

		KernelAction<CPUFactory,Input(CPUFactory::Image<Complex>),KernelOutput<0>> positionKernel;
		positionKernel.getKernelInput().setDefaultValue(
			factory.createKernel<CPUFactory::Image<Complex>>("position","positionKernel")
		);
		positionImageGenerator.output(0) >> positionKernel.input(4);
		imageRangeGenerator.output(0) >> positionKernel.getGlobalSizeInput();


	//calculation
		ImageGeneratorAction<CPUFactory,unsigned> mandelbrotImageGenerator(factory);
		sizeParam.output(0,1) >> mandelbrotImageGenerator.input(0,1);

		KernelAction<
			CPUFactory,
			Input(CPUFactory::Image<Complex>,CPUFactory::Image<unsigned>),
			KernelOutput<1>
		> mandelbrotKernel;
		mandelbrotKernel.getKernelInput().setDefaultValue(
			factory.createKernel<CPUFactory::Image<unsigned>>("calculation","mandelbrotKernel")
		);
		positionKernel.output(0) >> mandelbrotKernel.input(4);
		mandelbrotImageGenerator.output(0) >> mandelbrotKernel.input(5);
		imageRangeGenerator.output(0) >> mandelbrotKernel.getGlobalSizeInput();

	//coloring
		struct RGBA{
			unsigned char r,g,b,a;
		};
		ImageGeneratorAction<CPUFactory,RGBA> colorImageGenerator(factory);
		sizeParam.output(0,1) >> colorImageGenerator.input(0,1);

		KernelAction<
			CPUFactory,
			Input(CPUFactory::Image<unsigned>,CPUFactory::Image<RGBA>),
			KernelOutput<1>
		> coloringKernel;
		coloringKernel.getKernelInput().setDefaultValue(
			factory.createKernel<CPUFactory::Image<RGBA>>("calculation","mandelbrotKernel")
		);
		mandelbrotKernel.output(0) >> coloringKernel.input(4);
		colorImageGenerator.output(0) >> coloringKernel.input(5);
		imageRangeGenerator.output(0) >> coloringKernel.getGlobalSizeInput();

	_log("[info] done");
}




