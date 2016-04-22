#pragma once
#include "../Buffer.h"
#include "CL.h"

/*
 * CLBuffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct CLBuffer : Buffer<T>{
	CLBuffer(cl::Context& ctx,cl::CommandQueue queue,cl_mem_flags memFlags,size_t elemCount) :
		Buffer<T>(elemCount),
		queue(queue),
		bufferHandle(ctx,memFlags,sizeof(T) * elemCount){}

	T* getDataPointer(){
		throw std::runtime_error("not implemented");
	}
	const T* getDataPointer() const {
		throw std::runtime_error("not implemented");
	}

	cl::Buffer getHandle() const {
		return bufferHandle;
	}

	void copyToBuffer(std::vector<T>& buffer) const{
		buffer.resize(this->elemCount);
		queue.enqueueReadBuffer(bufferHandle,true,0,this->elemCount * sizeof(T),buffer.data());
		queue.finish();
	}
protected:
	cl::CommandQueue queue;
	cl::Buffer bufferHandle;
};





