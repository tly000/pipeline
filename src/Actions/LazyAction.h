#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * LazyAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */


template<typename...> struct LazyAction;

template<typename... Inputs,typename... Outputs>
struct LazyAction<Input(Inputs...),Output(Outputs...)> : StaticPipelineAction<Input(Inputs...),Output(Outputs...)>{

	virtual ~LazyAction() = default;
protected:
	bool first = true;
	std::tuple<Inputs...> lastValues;

	virtual void execute(){
		auto newValues = createInputTuple(std::index_sequence_for<Inputs...>{});
		if(first || lastValues != newValues){
			this->executeImpl();
			first = false;
		}
	}

	template<size_t... N> std::tuple<Inputs...> createInputTuple(std::index_sequence<N...>){
		return std::tuple<Inputs...>{
			this->template getInput<N>().getValue()...
		};
	}

	virtual void executeImpl() = 0;
};


