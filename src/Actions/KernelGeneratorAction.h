#pragma once
#include "../Actions/LazyAction.h"

/*
 * KernelGeneratorAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename Factory,typename... Inputs> struct KernelGeneratorAction
	: LazyAction<Input(std::string,std::string,std::string),Output(typename Factory::template Kernel<Val<Inputs>...>)>{

	KernelGeneratorAction(Factory factory) : factory(factory){
		this->template getInput<2>().setDefaultValue("");
	}
protected:
	Factory factory;

	void executeImpl(){
		std::string progName = this->template getInput<0>().getValue();
		std::string kernelName = this->template getInput<1>().getValue();
		std::string kernelParams = this->template getInput<2>().getValue();
		this->template getOutput<0>().setValue(factory.template createKernel<Val<Inputs>...>(progName,kernelName,kernelParams));
	}
};



