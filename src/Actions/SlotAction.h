#pragma once
#include "BoxedAction.h"

/*
 * SlotAction.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

template<typename...> struct SlotAction;

template<typename... Inputs, typename... Outputs> struct SlotAction<Input(Inputs...),Output(Outputs...)>
	: BoxedAction<Input(Inputs...),Output(Outputs...)>{

	SlotAction() : actionInput(this){}

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
		inputDelegation.outputs(I...) >> action->inputs(I...);
		action->outputs(J...) >> outputDelegation.inputs(I...);

		action->execute();

		variadicForEach(action->getInput<I>().disconnect());
		variadicForEach(outputDelegation->getInput<I>().disconnect());
	}
};


