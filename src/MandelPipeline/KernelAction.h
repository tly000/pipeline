#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * KernelAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename Factory,typename... Inputs>
struct KernelAction : StaticPipelineAction<Input(typename Factory::template Kernel<Inputs...>,Inputs...),Output()>{

	virtual ~KernelAction() = default;

	virtual void execute(){
		this->executeImpl(std::index_sequence_for<Inputs...>());
	}
private:
	template<size_t... N> void executeImpl(std::index_sequence<N...>){
		auto& kernel = this->template getInput<0>().getValue();
		kernel.run(this->template getInput<N+1>().getValue()...);
	}
};


