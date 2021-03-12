#pragma once
#include "../Actions/LazyAction.h"

/*
 * KernelGeneratorAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename Factory,typename... Inputs> struct KernelGeneratorAction
	: LazyAction<Input(
		KV("programName",std::string),
		KV("kernelName",std::string),
		KV("defines",std::string)
	),Output(
		KV("kernel",typename Factory::template Kernel<Val<Inputs>...>)
	)>{

	KernelGeneratorAction(Factory factory) : factory(factory){
		this->getInput(_C("defines")).setDefaultValue("");
	}
protected:
	Factory factory;

	void executeImpl(){
		std::string progName = this->getInput(_C("programName")).getValue();
		std::string kernelName = this->getInput(_C("kernelName")).getValue();
		std::string kernelParams = this->getInput(_C("defines")).getValue();
		this->template getOutput<0>().setValue(factory.template createKernel<Val<Inputs>...>(progName,kernelName,kernelParams));
	}
};



