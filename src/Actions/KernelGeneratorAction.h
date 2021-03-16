#pragma once
#include "../Actions/LazyAction.h"
#include "../Platform/Factory.h"

/*
 * KernelGeneratorAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename... Inputs> struct KernelGeneratorAction
	: LazyAction<Input(
		KV("programName",std::string),
		KV("kernelName",std::string),
		KV("defines",std::string)
	),Output(
		KV("kernel",Kernel<Val<Inputs>...>)
	)>{

	KernelGeneratorAction(Factory& factory) : factory(factory){
		this->getInput(_C("defines")).setDefaultValue("");
	}
protected:
	Factory& factory;

	void executeImpl(){
		std::string progName = this->getInput(_C("programName")).getValue();
		std::string kernelName = this->getInput(_C("kernelName")).getValue();
		std::string kernelParams = this->getInput(_C("defines")).getValue();
		this->template getOutput<0>().setValue(Kernel<Val<Inputs>...>(factory.createKernel(progName,kernelName,kernelParams)));
	}
};



