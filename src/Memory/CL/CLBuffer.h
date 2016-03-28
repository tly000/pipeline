#pragma once
#include "../Buffer.h"
#include <CL/cl.hpp>

/*
 * CLBuffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct CLBuffer : Buffer<T>{
	CLBuffer(size_t elemCount) :
		Buffer(elemCount){}
};





