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
	CLBuffer(cl::Context& ctx,cl_mem_flags memFlags,size_t elemCount) :
		Buffer(elemCount),
		bufferHandle(memFlags,sizeof(T) * elemCount){}
protected:
	cl::Buffer bufferHandle;
};





