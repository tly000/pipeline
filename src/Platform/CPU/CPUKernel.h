#pragma once
#include "../Kernel.h"

/*
 * CPUKernel.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename... Inputs>
struct CPUKernel : Kernel<Inputs...>{

	CPUKernel(void(* kernelFunc)(Inputs&...)) : kernelFunc(kernelFunc){}

	void run(Inputs&... inputs){
		kernelFunc(inputs...);
	}
protected:
	void(* kernelFunc)(Inputs&...);
};


