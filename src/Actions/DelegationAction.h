#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * DelegationAction.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

template<typename... T> struct DelegationAction : StaticPipelineAction<Input(T...),Output(T...)>{
protected:
	void execute(){
		this->executeImpl(std::index_sequence_for<T...>());
	}

	template<size_t... I> void executeImpl(std::index_sequence<I...>){
		variadicForEach(this->getOutput<I>().setValue(this->getInput<I>().getValue()));
	}
};


