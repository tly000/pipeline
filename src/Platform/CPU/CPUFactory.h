#pragma once
#include "CPUBuffer.h"
#include "CPUImage.h"
#include "CPUKernel.h"
#include <map>

#include "../../Kernel/CPU/position.h"
#include "../../Kernel/CPU/calculation.h"
#include "../../Kernel/CPU/coloring.h"
#include "../../Utility/DynamicLibrary.h"

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
		std::string filePath = "./src/Kernel/CPU/" + progName + ".cpp";
		std::string libPath = "./" + progName + ".lib";
		std::string output = systemCommand(
			"g++ -std=c++11 -shared \"" + filePath + "\" -o \"" + libPath + "\""
		);

		DynamicLibrary library(libPath);
		return CPUKernel<Inputs...>(
			library,
			reinterpret_cast<typename Kernel<Inputs...>::KernelFunc>(library.loadSymbol(kernelName))
		);
	}

	std::string getDeviceName() const {
		return "C++ on CPU";
	}
};


