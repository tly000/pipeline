#pragma once
#include "../Kernel.h"
#include "CL.h"
#include "../../Utility/VariadicUtils.h"

/*
 * CLKernel.h
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

template<typename T,typename...> struct CLArgumentForwarder{
	static T& forward(T& ref){
		return ref;
	}
};

template<typename... Inputs> struct CLKernel : Kernel<Inputs...>{
	CLKernel(cl::CommandQueue queue,cl::Kernel kernel)
		:queue(queue),
		 kernel(kernel){}

	void run(
		const Range& globalOffset,
		const Range& globalSize,
		const Range& localSize,
		Inputs&... inputs
	){
		int i = 0;
		variadicForEach(kernel.setArg(i++,CLArgumentForwarder<Inputs>::forward(inputs)));
		cl::NDRange offset(globalOffset.x,globalOffset.y,globalOffset.z);
		cl::NDRange global(globalSize.x,globalSize.y,globalSize.z);
		cl::NDRange local(localSize.x,localSize.y,localSize.z);
		queue.enqueueNDRangeKernel(kernel,offset,global,local);
		queue.finish();
	}
protected:
	cl::CommandQueue queue;
	cl::Kernel kernel;
};

template<typename T> struct CLArgumentForwarder<CLImage<T>>{
	static cl::Image2D forward(CLImage<T>& image){
		return image.getHandle();
	}
};

template<typename T> struct CLArgumentForwarder<CLBuffer<T>>{
	static cl::Buffer forward(CLBuffer<T>& buffer){
		return buffer.getHandle();
	}
};

