#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * FunctionCallAction.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

template<typename...> struct FunctionCallAction;

template<typename... Inputs,typename O> struct FunctionCallAction<Input(Inputs...),O>
	: StaticPipelineAction<Input(Inputs...),Output(O)>{
	FunctionCallAction(std::function<O(const Inputs&...)> func)
		:func(func){}
protected:
	std::function<O(const Inputs&...)> func;

	void execute(){
		this->executeImpl(std::index_sequence_for<Inputs...>());
	}

	template<size_t... I> void executeImpl(std::index_sequence<I...>){
		this->template getOutput<0>().setValue(func(this->template getInput<I>().getValue()...));
	}
};


