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
	std::tuple<Val<Inputs>...> lastValues;

	virtual void execute(){
		auto newValues = createInputTuple(std::index_sequence_for<Val<Inputs>...>{});
		if(first || lastValues != newValues){
			this->executeImpl();
			first = false;
			lastValues = newValues;
		} else{
			_logDebug("lazy execution of " << demangle(typeid(*this)) << " skipped.");
		}
	}

	template<size_t... N> std::tuple<Val<Inputs>...> createInputTuple(std::index_sequence<N...>){
		return std::tuple<Val<Inputs>...>{
			this->template getInput<N>().getValue()...
		};
	}

	virtual void executeImpl() = 0;
};


