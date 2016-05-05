#pragma once

#include "../Actions/GeneratorAction.h"
#include "../Actions/ImageGeneratorAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Type/Vec.h"
#include "../UI/UIParameterAction.h"
#include "PipelineWrapper.h"
#include "../Type/TypeHelper.h"
#include "KernelDefinesAction.h"

/*
 * WrappedMandelPipeline.h
 *
 *  Created on: 03.05.2016
 *      Author: tly
 */

template<typename Factory,typename T>
struct WrappedMandelPipeline : PipelineWrapper{
	using U32Image = typename Factory::template Image<uint32_t>;
	using ComplexImage = typename Factory::template Image<Vec<2,T>>;

	WrappedMandelPipeline(Factory& factory,const std::string& typeName):
		positionImageGenerator(factory),
		positionKernelGenerator(factory),
		mandelbrotImageGenerator(factory),
		mandelbrotKernelGenerator(factory),
		colorImageGenerator(factory),
		coloringKernelGenerator(factory){
		_log("[info] creating pipeline with type " << typeName << " and platform " << factory.getDeviceName());

		sizeParam.setValue<0>(512);//width
		sizeParam.setValue<1>(512);//height

		sizeParam.output(0,1) >> imageRangeGenerator.input(0,1);
		imageRangeGenerator.getInput<2>().setDefaultValue(1);

		iterationParam.setValue<0>(64);//iterations
		iterationParam.setValue<1>(4);//bailout²

		iterationParam.output(0,1) >> toStringAction.input(0,1);
		toStringAction.output(0,1) >> kernelDefinesAction.input(0,1);
		kernelDefinesAction.template getInput<2>().setDefaultValue(typeName);
		//position
			positionParam.template setValue<0>(fromFloatToType<T>(-0.5));
			positionParam.template setValue<1>(fromFloatToType<T>(0));
			positionParam.template setValue<2>(fromFloatToType<T>(2));

			sizeParam.output(0,1) >> positionImageGenerator.input(0,1);
			positionKernelGenerator.template getInput<0>().setDefaultValue("position");
			positionKernelGenerator.template getInput<1>().setDefaultValue("positionKernel");
			kernelDefinesAction.output(0) >> positionKernelGenerator.input(2);
			positionKernelGenerator.output(0) >> positionKernel.getKernelInput();

			positionImageGenerator.output(0) >> positionKernel.kernelInput(0);
			positionParam.output(0,1,2) >> positionKernel.kernelInput(1,2,3);
			imageRangeGenerator.output(0) >> positionKernel.getGlobalSizeInput();
		//calculation
			sizeParam.output(0,1) >> mandelbrotImageGenerator.input(0,1);

			mandelbrotKernelGenerator.template getInput<0>().setDefaultValue("calculation");
			mandelbrotKernelGenerator.template getInput<1>().setDefaultValue("mandelbrotKernel");
			kernelDefinesAction.output(0) >> mandelbrotKernelGenerator.input(2);
			mandelbrotKernelGenerator.output(0) >> mandelbrotKernel.getKernelInput();

			positionKernel.output(0) >> mandelbrotKernel.kernelInput(0);
			mandelbrotImageGenerator.output(0) >> mandelbrotKernel.kernelInput(1);
			imageRangeGenerator.output(0) >> mandelbrotKernel.getGlobalSizeInput();
		//coloring
			sizeParam.output(0,1) >> colorImageGenerator.input(0,1);

			coloringKernelGenerator.template getInput<0>().setDefaultValue("coloring");
			coloringKernelGenerator.template getInput<1>().setDefaultValue("coloringKernel");
			kernelDefinesAction.output(0) >> coloringKernelGenerator.input(2);
			coloringKernelGenerator.output(0) >> coloringKernel.getKernelInput();

			mandelbrotKernel.output(0) >> coloringKernel.kernelInput(0);
			colorImageGenerator.output(0) >> coloringKernel.kernelInput(1);
			imageRangeGenerator.output(0) >> coloringKernel.getGlobalSizeInput();

		//adding parameters
			this->addParam(this->sizeParam);
			this->addParam(this->positionParam);
			this->addParam(this->iterationParam);
	}

	virtual void run(){
		_log("[info] running pipeline " + demangle(typeid(*this)));
		Timer timer;

		timer.start();
		coloringKernel.run();
		auto fullTime = timer.stop();
		_log("[info] position: " << positionKernel.template getOutput<1>().getValue());
		_log("[info] calculation: " << mandelbrotKernel.template getOutput<1>().getValue());
		_log("[info] coloring: " << coloringKernel.template getOutput<1>().getValue());
		_log("[info] full: " << fullTime << " us.");
	}

	const U32Image& getRenderedImage(){
		return coloringKernel.template getOutput<0>().getValue();
	}

	UIParameterAction<T,T,T>& getPositionParam(){
		return positionParam;
	}
protected:
	//general actions:
		//size of the generated image
		UIParameterAction<uint32_t,uint32_t> sizeParam{"image dimensions","width","height"};
		//generates a Range object, fitting the sizeParam
		GeneratorAction<Input(uint32_t,uint32_t,uint32_t),Output(Range)> imageRangeGenerator;
		//generates defines used to compile the kernels
		KernelDefinesAction<3> kernelDefinesAction{"MAXITER","BAILOUT","Type"};
		//generates strings for the defines
		FunctionAction<Input(unsigned,float),Output(std::string,std::string)> toStringAction{
			&toString<unsigned>, &toString<float>
		};
	//position kernel
		UIParameterAction<T,T,T> positionParam{"position", "offsetReal", "offsetImag" ,"scale"};
		//generates the position image, holding the complex coordinates that the mandelbrotkernel uses
		ImageGeneratorAction<Factory,Vec<2,T>> positionImageGenerator;
		KernelGeneratorAction<Factory,ComplexImage,T,T,T> positionKernelGenerator;
		//kernel to calculate the complex coordinates used later for the calculation
		//input: one ComplexImage to be filled by the kernel
		//output: input 0 (the ComplexImage)
		KernelAction<
			Factory,
			Input(ComplexImage positionOutput,T offsetReal,T offsetImag,T scale),
			KernelOutput<0>
		> positionKernel;
	//mandelbrot kernel
		UIParameterAction<unsigned,float> iterationParam{"iteration", "iterations", "bailout"};
		//generates an image of size "sizeParam"
		ImageGeneratorAction<Factory,unsigned> mandelbrotImageGenerator;
		KernelGeneratorAction<Factory,ComplexImage,U32Image> mandelbrotKernelGenerator;
		//the main mandelbrot calculation
		//input: ComplexImage holding complex coordinates, U32Image to save the calculated iteration count
		//output: input 1 (the iteration count image)
		KernelAction<
			Factory,
			Input(ComplexImage positions,U32Image iterationOutput),
			KernelOutput<1>
		> mandelbrotKernel;
	//coloring kernel
		//generates an image of size "sizeParam" holding RGBA values
		ImageGeneratorAction<Factory,unsigned> colorImageGenerator;
		KernelGeneratorAction<Factory,U32Image,U32Image> coloringKernelGenerator;
		//the coloring kernel
		//input: U32Image holding the iteration count, U32Image to write the color data
		//output: input 1 (the colored image)
		KernelAction<
			Factory,
			Input(U32Image iterationInput,U32Image colorOutput),
			KernelOutput<1>
		> coloringKernel;
};


