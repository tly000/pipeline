#pragma once
#include "../Actions/BoxedAction.h"
#include "ImageGeneratorAction.h"
#include "KernelAction.h"
#include "../Actions/ParameterAction.h"

/*
 * PositionAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename Factory,
		 typename T,
		 typename ImageType = typename Factory::template Image<T>,
		 typename KernelType= typename Factory::template Kernel<ImageType>>
struct PositionAction : BoxedAction<Input(uint32_t,uint32_t),Output(ImageType)>{

	PositionAction(Factory factory = Factory())
		: factory(factory),
		  positionImageGenerator(factory){
		this->setInputs(
			positionImageGenerator.template getInput<0>(),
			positionImageGenerator.template getInput<1>()
		);
		this->setOutputs(
			kernelAction.template getOutput<0>()
		);

		kernelAction.getKernelInput().setDefaultValue(
			factory.template createKernel<ImageType>("position","positionKernel")
		);
		positionImageGenerator.template connectTo(kernelAction,IntPair<0,4>{});
	}
protected:
	Factory factory;

	ImageGeneratorAction<Factory,T> positionImageGenerator;

	KernelAction<Factory,Input(ImageType),KernelOutput<0>> kernelAction;

	void execute(){
		kernelAction.execute();
	}
};



