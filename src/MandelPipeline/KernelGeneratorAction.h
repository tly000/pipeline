#pragma once
#include "../Actions/LazyAction.h"

/*
 * KernelGeneratorAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename Factory,typename... Inputs> struct KernelGeneratorAction
	: LazyAction<Input(std::string,std::string),Output(Factory::Kernel<Inputs...>)>{

	KernelGeneratorAction(Factory factory = Factory()) : factory(factory){}
protected:
	Factory factory;

	void executeImpl(){
		std::string progName = this->template getInput<0>().getValue();
		std::string kernelName = this->template getInput<1>().getValue();
		this->template getOutput<0>().setValue(factory.createKernel(progName,kernelName));
	}
};



