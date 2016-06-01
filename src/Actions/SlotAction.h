#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * SlotAction.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

template<typename...> struct SlotAction;

template<typename... Inputs, typename... Outputs> struct SlotAction<Input(Inputs...),Output(Outputs...)>
	: StaticPipelineAction<Input(Inputs...),Output(Outputs...)>{

	SlotAction() : actionInput(this,"actioninput"){}

	auto& getActionInput(){
		return actionInput;
	}

protected:
	StaticInput<StaticPipelineAction<Input(Inputs...),Output(Outputs...)>*> actionInput;

	void execute(){
		this->executeImpl(std::index_sequence_for<Inputs...>(),std::index_sequence_for<Outputs...>());
	}

	template<size_t... I, size_t... J> void executeImpl(std::index_sequence<I...>,std::index_sequence<J...>){
		auto* action = actionInput.getValue();
		variadicForEach(action->template getInput<I>().setDefaultValue(this->template getInput<I>().getValue()));
		action->run();
		variadicForEach(this->template getOutput<J>().setValue(action->template getOutput<J>().getValue()));
	}
};


