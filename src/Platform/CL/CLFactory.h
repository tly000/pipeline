#pragma once
#include "CLBuffer.h"
#include "CLImage.h"

/*
 * CLFactory.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

struct CLFactory{
	CLFactory(...){
		//TODO
	}

	template<typename T> using Image = CLImage<T>;
	template<typename T> using Buffer= CLBuffer<T>;
	template<typename... Inputs> using Kernel = cl::Kernel;

	template<typename T> Image<T> createImage(uint32_t width,uint32_t height){
		return CLImage<T>(width,height);
	}
	template<typename T> Buffer<T> createBuffer(uint32_t elemCount){
		return CLBuffer<T>(elemCount);
	}
	template<typename T> Buffer<T> createKernel(const std::string& progName,const std::string& kernelName){
		//TODO load program?
	}
};


