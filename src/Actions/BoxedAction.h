#pragma once
#include "DelegationAction.h"

/*
 * BoxedAction.h
 *
 *  Created on: 30.03.2016
 *      Author: tly
 */

template<typename...> struct BoxedAction;

template<typename... Inputs,typename... Outputs>
struct BoxedAction<Input(Inputs...),Output(Outputs...)>
	: AbstractPipelineAction, NonCopyable{

	BoxedAction(){}

	template<size_t N> auto& getInput();
	template<size_t N> auto& getOutput();

	template<typename Action,int... As,int... Bs>
	void connectTo(Action& action,IntPair<As,Bs>...);

protected:
	template<size_t N> auto& getInternalInput();
	template<size_t N> auto& getInternalOutput();
private:
	DelegationAction<Inputs...> inputDelegation;
	DelegationAction<Outputs...> outputDelegation;

	template<size_t... N> void addOutputs(std::index_sequence<N...>){
		variadicForEach(this->outputs.push_back(&outputDelegation.getOutput<N>()));
	}

	template<size_t... N> void addInputs(std::index_sequence<N...>){
		variadicForEach(this->inputs.push_back(&inputDelegation.getInput<N>()));
	}

};

template<typename... Inputs,typename... Outputs>
template<typename Action,int... As,int... Bs>
void BoxedAction<Input(Inputs...),Output(Outputs...)>::connectTo(Action& action,IntPair<As,Bs>...){
	variadicForEach(this->getOutput<As>().connectTo(action.getInput<Bs>()));
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& BoxedAction<Input(Inputs...), Output(Outputs...)>::getInput(){
	return inputDelegation.getInput<N>();
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& BoxedAction<Input(Inputs...), Output(Outputs...)>::getOutput(){
	return outputDelegation.getOutput<N>();
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& BoxedAction<Input(Inputs...), Output(Outputs...)>::getInternalInput(){
	return inputDelegation.getOutput<N>();
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& BoxedAction<Input(Inputs...), Output(Outputs...)>::getInternalOutput(){
	return outputDelegation.getInput<N>();
}
