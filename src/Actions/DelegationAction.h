#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * DelegationAction.h
 *
 *  Created on: 30.03.2016
 *      Author: tly
 */

template<typename... Inputs> struct DelegationAction
	: StaticPipelineAction<Input(Inputs...),Output(Inputs...)>{
protected:
	void execute(){
		executeImpl(std::index_sequence_for<Inputs...>{});
	}

	template<size_t... N> void executeImpl(std::index_sequence<N...>){
		variadicForEach(
			this->template getOutput<N>().getValue() = this->template getInput<N>().getValue()
		);
	}
};



