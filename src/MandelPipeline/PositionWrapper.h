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
		positionParam.template getParam<0>().setValueFromString("-0.5");
		positionParam.template getParam<1>().setValueFromString("0.0");
		positionParam.template getParam<2>().setValueFromString("2.0");
		positionParam.template getParam<3>().setValueFromString("0");

		pipeline->multisampleSizeParam.output(0) >> positionImageGenerator.input(0);

		positionKernelGenerator.template getInput<0>().setDefaultValue("position");
		positionKernelGenerator.template getInput<1>().setDefaultValue("positionKernel");
		pipeline->kernelDefinesAction.output(0) >> positionKernelGenerator.input(2);
		positionKernelGenerator.output(0) >> positionKernel.getKernelInput();

		positionImageGenerator.output(0) >> positionKernel.kernelInput(0);
		pipeline->multisampleSizeParam.output(0) >> positionKernel.getGlobalSizeInput();

		positionParam.output(0,1) >> positionKernel.kernelInput(1,2);
		positionParam.output(2,3) >> calculateRotationAction.input(0,1);
		calculateRotationAction.output(0) >> positionKernel.kernelInput(3);
		pipeline->addParam(positionParam);
	}

	UIParameterAction<T,T,T,float> positionParam{"position","real position","imag position","scale","rotation"};
	//generates an image of size "multisampleSizeParam"
	ImageGeneratorAction<Factory,Vec<2,T>> positionImageGenerator;
	KernelGeneratorAction<Factory,ComplexImage,T,T,Vec<2,T>> positionKernelGenerator;
	//the position kernel
	//input: ComplexImage to be filled with position data, T offsetReal, T offsetImage, T scaleReal, T scaleImag
	//output: input 0 (the filled image)
	KernelAction <
		Factory,
		Input(ComplexImage positionImage,T,T,Vec<2,T>),
		KernelOutput<0>
	> positionKernel;

	FunctionCallAction<Input(T,float),Vec<2,T>> calculateRotationAction{
		[](const T& scale,const float& angle){
			float rad = angle / 180 * 3.14159;
			return Vec<2,T>{{
				tmul(scale,fromFloatToType<T>(std::cos(rad))),
				tmul(scale,fromFloatToType<T>(std::sin(rad)))
			}};
		}
	};
};




