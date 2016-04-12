#pragma once
#include "../Buffer.h"
#include <vector>

/*
 * CPUBuffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct CPUBuffer : Buffer<T>{
	CPUBuffer(size_t elemCount)
	  : Buffer<T>(elemCount),
		data(std::make_shared<std::vector<T>>(elemCount)){}

	T* getDataPointer(){ return data->data(); }
	const T* getDataPointer() const{ return data->data(); }
protected:
	std::shared_ptr<std::vector<T>> data;
};


