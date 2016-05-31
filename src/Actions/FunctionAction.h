#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * FunctionAction.h
 *
 *  Created on: 05.05.2016
 *      Author: tly
 */

template<typename...> struct FunctionAction;

template<typename... Inputs,typename... Outputs> struct FunctionAction<Input(Inputs...),Output(Outputs...)>
: StaticPipelineAction<Input(Inputs...),Output(Outputs...)>{
	FunctionAction(Val<Outputs>(* ...functions)(const Val<Inputs>&)):
		functions{functions...}{}
protected:
	std::tuple<Val<Outputs>(*)(const Val<Inputs>&)...> functions;

	void execute(){
		this->executeImpl(std::index_sequence_for<Inputs...>());
	}

	template<size_t... I> void executeImpl(std::index_sequence<I...>){
		variadicForEach(this->template getOutput<I>().setValue(
			std::get<I>(functions)(this->template getInput<I>().getValue())
		));
	}
};

