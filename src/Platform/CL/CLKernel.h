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
	static void forward(cl::Kernel& k,int& i,T& ref){
		k.setArg(i,ref);
		i++;
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
		variadicForEach(CLArgumentForwarder<Inputs>::forward(kernel,i,inputs));
		cl::NDRange offset(globalOffset.x,globalOffset.y,globalOffset.z);
		cl::NDRange global(globalSize.x,globalSize.y,globalSize.z);
		cl::NDRange local(localSize.x,localSize.y,localSize.z);
		queue.enqueueNDRangeKernel(kernel,offset,global/*,local*/);
		queue.finish();
	}
protected:
	cl::CommandQueue queue;
	cl::Kernel kernel;
};

template<typename T> struct CLArgumentForwarder<CLImage<T>>{
	static void forward(cl::Kernel& k,int& i,CLImage<T>& image){
		k.setArg(i,image.getHandle());
		i++;
		k.setArg(i,uint32_t(image.getWidth()));
		i++;
		k.setArg(i,uint32_t(image.getHeight()));
		i++;
	}
};

template<typename T> struct CLArgumentForwarder<CLBuffer<T>>{
	static void forward(cl::Kernel& k,int& i,CLBuffer<T>& buffer){
		k.setArg(i,buffer.getHandle());
		i++;
	}
};

