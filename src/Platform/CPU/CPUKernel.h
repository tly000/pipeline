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
	using KernelFunc = void(*)(const Range& globalID,const Range& localID,Inputs&...);

	CPUKernel(KernelFunc kernelFunc) : kernelFunc(kernelFunc){}

	void run(
		const Range& globalOffset,
		const Range& globalSize,
		const Range& localSize,
		Inputs&... inputs){
		for(unsigned l = 0; l < localSize.x; l++){
			for(unsigned m = 0; m < localSize.y; m++){
				for(unsigned n = 0; n < localSize.z; n++){

					for(auto i = globalOffset.x; i < globalSize.x; i++){
						for(auto j = globalOffset.y; j < globalSize.y; j++){
							for(auto k = globalOffset.z; k < globalSize.z; k++){
								kernelFunc(Range{i,j,k},Range{l,m,n},inputs...);
							}
						}
					}

				}
			}
		}
	}
protected:
	KernelFunc kernelFunc;
};


