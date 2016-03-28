#pragma once
#include "../Buffer.h"
#include <memory>

/*
 * CPUBuffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct CPUBuffer : Buffer<T>{
	CPUBuffer(size_t elemCount)
	  : Buffer(elemCount),
		data(std::make_unique<T[]>(elemCount)){}

	T* getDataPointer(){ return data.get(); }
protected:
	std::unique_ptr<T[]> data;
};


