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
//#include "../Kernel/Type/Q16_16.h"
#include "../Kernel/Type/Fixed4.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"
/*
 * main.cpp
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename T,typename Factory> void runPipeline(const std::string& typeName,Factory& factory){
	BackTracer::registerErrorHandler();
	using U32Image = typename Factory::template Image<uint32_t>;
	using ComplexImage = typename Factory::template Image<Vec<2,T>>;

	_log("[info] creating pipeline with type " << typeName << " and platform " << factory.getDeviceName());

	ParameterAction<uint32_t,uint32_t> sizeParam;
	sizeParam.setValue<0>(512);
	sizeParam.setValue<1>(512);
	std::string compilerParams =
		"-DMAXITER=64 "
		"-DBAILOUT=4 "
		"-DType="+typeName;

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
	_log("[info] 1st time: ");
	_log("[info] position: " << positionKernel.template getOutput<1>().getValue());
	_log("[info] calculation: " << mandelbrotKernel.template getOutput<1>().getValue());
	_log("[info] coloring: " << coloringKernel.template getOutput<1>().getValue());
	_log("[info] full: " << timer.stop() << " us.");
	timer.start();
	coloringKernel.run();
	_log("[info] 2nd time: ");
	_log("[info] position: " << positionKernel.template getOutput<1>().getValue());
	_log("[info] calculation: " << mandelbrotKernel.template getOutput<1>().getValue());
	_log("[info] coloring: " << coloringKernel.template getOutput<1>().getValue());
	_log("[info] full: " << timer.stop() << " us.");

	const auto& coloredImage = coloringKernel.template getOutput<0>().getValue();
	std::vector<unsigned> data;
	coloredImage.copyToBuffer(data);
	std::string fileName = "test_" + typeName + ".bmp";
	stbi_write_bmp(fileName.c_str(),sizeParam.getValue<0>(),sizeParam.getValue<1>(),4,data.data());
}

int main(){
	CLFactory gpu(1);
	CPUFactory cpu;
	CLFactory cpu2(2);
	runPipeline<float,CLFactory>("float",gpu);
	runPipeline<double,CLFactory>("double",gpu);
	runPipeline<Fixed4,CLFactory>("Fixed4",gpu);

	runPipeline<float,CPUFactory>("float",cpu);
	runPipeline<double,CPUFactory>("double",cpu);
	runPipeline<Fixed4,CPUFactory>("Fixed4",cpu);

	runPipeline<float,CLFactory>("float",cpu2);
	runPipeline<double,CLFactory>("double",cpu2);
	runPipeline<Fixed4,CLFactory>("Fixed4",cpu2);
}




