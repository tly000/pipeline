#include "../Actions/ParameterAction.h"
#include "../Platform/CPU/CPUFactory.h"
#include "../Platform/CL/CLFactory.h"
#include "ImageGeneratorAction.h"
#include "KernelGeneratorAction.h"
#include "KernelAction.h"
#include "../Actions/GeneratorAction.h"
#include "../Type/Vec.h"
#include <BackTracer.h>
#include "../Utility/Timer.h"
#include "../Kernel/Type/Q16_16.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"
/*
 * main.cpp
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename T,typename Factory> void runPipeline(){
	BackTracer::registerErrorHandler();
	Factory factory;
	using U32Image = typename Factory::template Image<uint32_t>;
	using ComplexImage = typename Factory::template Image<Vec<2,T>>;

	_log("[info] creating pipeline with " << demangle(typeid(Factory)) << " using \"" << factory.getDeviceName() << "\".");

	ParameterAction<uint32_t,uint32_t> sizeParam;
	sizeParam.setValue<0>(512);
	sizeParam.setValue<1>(512);
	std::string compilerParams =
		"-DMAXITER=64 "
		"-DBAILOUT=4 "
		"-DType="+demangle(typeid(T));

	GeneratorAction<Input(uint32_t,uint32_t,uint32_t),Output(Range)> imageRangeGenerator;
	sizeParam.output(0,1) >> imageRangeGenerator.input(0,1);
	imageRangeGenerator.getInput<2>().setDefaultValue(1);

	//position
		ImageGeneratorAction<Factory,Vec<2,T>> positionImageGenerator(factory);
		sizeParam.output(0,1) >> positionImageGenerator.input(0,1);

		KernelAction<Factory,Input(ComplexImage),KernelOutput<0>> positionKernel;
		positionKernel.getKernelInput().setDefaultValue(
			factory.template createKernel<ComplexImage>("position","positionKernel",compilerParams)
		);
		positionImageGenerator.output(0) >> positionKernel.input(4);
		imageRangeGenerator.output(0) >> positionKernel.getGlobalSizeInput();

	//calculation
		ImageGeneratorAction<Factory,unsigned> mandelbrotImageGenerator(factory);
		sizeParam.output(0,1) >> mandelbrotImageGenerator.input(0,1);

		KernelAction<
			Factory,
			Input(ComplexImage,U32Image),
			KernelOutput<1>
		> mandelbrotKernel;
		mandelbrotKernel.getKernelInput().setDefaultValue(
			factory.template createKernel<ComplexImage,U32Image>("calculation","mandelbrotKernel",compilerParams)
		);
		positionKernel.output(0) >> mandelbrotKernel.input(4);
		mandelbrotImageGenerator.output(0) >> mandelbrotKernel.input(5);
		imageRangeGenerator.output(0) >> mandelbrotKernel.getGlobalSizeInput();

	//coloring
		ImageGeneratorAction<Factory,unsigned> colorImageGenerator(factory);
		sizeParam.output(0,1) >> colorImageGenerator.input(0,1);

		KernelAction<
			Factory,
			Input(U32Image,U32Image),
			KernelOutput<1>
		> coloringKernel;
		coloringKernel.getKernelInput().setDefaultValue(
			factory.template createKernel<U32Image,U32Image>("coloring","coloringKernel",compilerParams)
		);
		mandelbrotKernel.output(0) >> coloringKernel.input(4);
		colorImageGenerator.output(0) >> coloringKernel.input(5);
		imageRangeGenerator.output(0) >> coloringKernel.getGlobalSizeInput();

	Timer timer;

	timer.start();
	coloringKernel.run();
	_log("[info] 1st time: " << timer.stop() << " us.");
	timer.start();
	coloringKernel.run();
	_log("[info] 2nd time: " << timer.stop() << " us.");

	const auto& coloredImage = coloringKernel.template getOutput<0>().getValue();
	//stbi_write_bmp("test.bmp",512,512,4,coloredImage.getDataPointer());
}

int main(){
	runPipeline<float,CLFactory>();
	runPipeline<float,CPUFactory>();
}




