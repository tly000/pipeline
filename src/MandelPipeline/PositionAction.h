#pragma once
#include "../Actions/BoxedAction.h"
#include "ImageGeneratorAction.h"
#include "KernelAction.h"

/*
 * PositionAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename Factory,typename T>
struct PositionAction : BoxedAction<Input(uint32_t,uint32_t),Output(typename Factory::template Image<T>)>{

	PositionAction(Factory factory = Factory())
		: factory(factory),
		  positionImageGenerator(factory){
		this->setInputs(
			positionImageGenerator.template getInput<0>(),
			positionImageGenerator.template getInput<1>()
		);
		this->setOutputs(
			positionImageGenerator.template getOutput<0>()
		);
	}
protected:
	Factory factory;
	ImageGeneratorAction<Factory,T> positionImageGenerator;
	KernelAction<Factory,typename Factory::template Image<T>> kernelAction;

	void execute(){
		kernelAction.execute();
	}
};



