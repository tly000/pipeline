#pragma once
#include "CPUBuffer.h"
#include "CPUImage.h"
#include "CPUKernel.h"
#include "../../Utility/DynamicLibrary.h"

#include <map>
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
	template<typename... Inputs> Kernel<Inputs...> createKernel(const std::string& progName,const std::string& kernelName,const std::string& compilerParams){
		std::string filePath = "./src/Kernel/CPU/" + progName + ".cpp";

		std::string libName = progName;
		int i = 0;
		while(fileExists(libName + ".lib")){
			libName = progName + std::to_string(i);
			i++;
		}
		std::string libPath = libName + ".lib";
		std::pair<int,std::string> output = systemCommand(
			"g++ -O3 -shared -std=c++11 " + compilerParams +
			" \"" + filePath + "\""
			" -o \"" + libPath + "\""
		);
		if(output.first){
			throw std::runtime_error("error compiling program " + filePath + " :\n" + output.second);
		} else {
			_log("[info] successfully compiled " << filePath << ".");
		}

		DynamicLibrary library(getCurrentWorkingDirectory() + "/" + libPath);
		return CPUKernel<Inputs...>(
			library,
			reinterpret_cast<typename Kernel<Inputs...>::KernelFunc>(library.loadSymbol(kernelName))
		);
	}

	std::string getDeviceName() const {
		return "C++ on CPU";
	}
};


