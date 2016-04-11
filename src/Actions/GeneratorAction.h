#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * GeneratorAction.h
 *
 *  Created on: 28.03.2016
 *      Author: tly
 */

template<typename...> struct GeneratorAction;

template<typename... Inputs,typename T>
struct GeneratorAction<Input(Inputs...),Output(T)> : StaticPipelineAction<Input(Inputs...),Output(T)>{
	virtual ~GeneratorAction() = default;
protected:
	virtual void execute(){
		executeImpl(std::index_sequence_for<Inputs>{});
	}

	template<size_t... N> void executeImpl(std::index_sequence<N>){
		this->template getOutput<0>.getValue() = T(
			this->template getInput<N>.getValue()...
		);
	}
};

