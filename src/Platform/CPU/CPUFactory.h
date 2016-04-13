#pragma once
#include "CPUBuffer.h"
#include "CPUImage.h"
#include "CPUKernel.h"
#include <map>

#include "../../Kernel/CPU/position.h"

/*
 * CPUFactory.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

struct CPUFactory{
	template<typename T> using Image = CPUImage<T>;
	template<typename T> using Buffer= CPUBuffer<T>;
	template<typename... Inputs> using Kernel = CPUKernel<Inputs...>;

	template<typename T> Image<T> createImage(uint32_t width,uint32_t height){
		return CPUImage<T>(width,height);
	}
	template<typename T> Buffer<T> createBuffer(uint32_t elemCount){
		return CPUBuffer<T>(elemCount);
	}
	template<typename... Inputs> Kernel<Inputs...> createKernel(const std::string& progName,const std::string& kernelName){
		auto it = internalKernelFunctions.find({progName,kernelName});
		if(it != internalKernelFunctions.end()){
			//unsafe cast, but impossible to handle otherwise
			return CPUKernel<Inputs...>(reinterpret_cast<typename CPUKernel<Inputs...>::KernelFunc>(it->second));
		}else{
			throw std::runtime_error("kernel function not found");
		}
	}
protected:
	std::map<std::pair<std::string,std::string>,void*> internalKernelFunctions = {
		{
			{"position","positionKernel"}, reinterpret_cast<void*>(positionKernel)
		}
	};
};


