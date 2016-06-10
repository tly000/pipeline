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
		variadicForEach(this->forward(i,inputs));
		cl::NDRange offset(globalOffset.x,globalOffset.y,globalOffset.z);
		cl::NDRange global(globalSize.x,globalSize.y,globalSize.z);
		cl::NDRange local(localSize.x,localSize.y,localSize.z);
		queue.enqueueNDRangeKernel(kernel,offset,global/*,local*/);
		queue.finish();
	}
protected:
	cl::CommandQueue queue;
	cl::Kernel kernel;

	template<typename T> void forward(int& i,CLImage<T>& image){
		kernel.setArg(i,image.getHandle());
		i++;
		kernel.setArg(i,uint32_t(image.getWidth()));
		i++;
		kernel.setArg(i,uint32_t(image.getHeight()));
		i++;
	}

	template<typename T> void forward(int& i,CLBuffer<T>& buffer){
		kernel.setArg(i,buffer.getHandle());
		i++;
	}

	template<typename T> void forward(int& i,T& obj){
		kernel.setArg(i,obj);
		i++;
	}
};

template<typename T> struct CLArgumentForwarder<CLImage<T>>{

};

template<typename T> struct CLArgumentForwarder<CLBuffer<T>>{

};

