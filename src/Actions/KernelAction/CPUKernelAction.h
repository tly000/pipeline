#pragma once
#include "KernelAction.h"
#include <string>

/*
 * CPUKernelAction.h
 *
 *  Created on: 10.04.2016
 *      Author: tly
 */

template<typename... Inputs>
struct CPUKernelAction : KernelAction<void(*)(Inputs&...),Inputs...>{
	virtual ~CPUKernelAction() = default;
protected:
	virtual void execute(){
		auto kernelFunc = this->template getInput<0>().getValue();
		this->executeImpl(kernelFunc,std::index_sequence_for<Inputs...>());
	}
private:
	template<size_t... N> void executeImpl(void(* kernelFunc)(Inputs&...),std::index_sequence<N...>){
		kernelFunc(this->template getInput<N+1>().getValue()...);
	}
};




