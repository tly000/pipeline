#pragma once
#include "KernelAction.h"
#include <string>
#include <CL/cl.hpp>

/*
 * CLKernelAction.h
 *
 *  Created on: 29.03.2016
 *      Author: tly
 */

template<typename... Inputs>
struct CLKernelAction : KernelAction<cl::Kernel,Inputs...>{

	CLKernelAction(const cl::CommandQueue& queue)
		: queue(queue){
		int argCount = kernel.getInfo<CL_KERNEL_NUM_ARGS>();
		if(argCount != sizeof...(Inputs)){
			std::cerr << "warning: CLKernelAction argument count does not match the argument count of " << kernelName << std::endl;
		}
	}

	virtual ~CLKernelAction() = default;
protected:
	cl::CommandQueue queue;

	virtual void execute(){
		bindArgs(std::index_sequence_for<Inputs...>{});
		//TODO real ranges
		queue.enqueueNDRangeKernel(kernel,cl::NDRange(0,0),cl::NDRange(100,100));
		//TODO finish queue?
		queue.finish();
	}

	template<size_t... N> void bindArgs(std::index_sequence<N...>){
		variadicForEach(kernel.setArg(N,this->template getInput<N>().getValue()));
	}
};






