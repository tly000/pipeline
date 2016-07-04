#pragma once
#include "../Kernel.h"
#include "../../Utility/DynamicLibrary.h"

/*
 * CPUKernel.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<bool EnableOpenMP,typename... Inputs>
struct CPUKernel : Kernel<Inputs...>{
	using KernelFunc = void(*)(const Range& globalID,const Range& localID,Inputs&...);

	CPUKernel(DynamicLibrary library,KernelFunc kernelFunc)
		: library(library),kernelFunc(kernelFunc){}

	void run(
		const Range& globalOffset,
		const Range& globalSize,
		const Range& localSize,
		Inputs&... inputs){

		if(EnableOpenMP){
			#pragma omp parallel for collapse(3)
			for(auto i = globalOffset.x; i < globalOffset.x + globalSize.x; i++){
				for(auto j = globalOffset.y; j < globalOffset.y + globalSize.y; j++){
					for(auto k = globalOffset.z; k < globalOffset.z + globalSize.z; k++){
						kernelFunc(Range{i,j,k},Range{0,0,0},inputs...);
					}
				}
			}
		}else{
			for(auto i = globalOffset.x; i < globalOffset.x + globalSize.x; i++){
				for(auto j = globalOffset.y; j < globalOffset.y + globalSize.y; j++){
					for(auto k = globalOffset.z; k < globalOffset.z + globalSize.z; k++){
						kernelFunc(Range{i,j,k},Range{0,0,0},inputs...);
					}
				}
			}
		}
	}
protected:
	DynamicLibrary library;
	KernelFunc kernelFunc;
};


