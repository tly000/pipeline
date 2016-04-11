#pragma once
#include "../Pipeline/StaticPipelineAction.h"

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

	template<size_t N> auto& getInput();
	template<size_t N> auto& getOutput();

	template<typename Action,int... As,int... Bs>
	void connectTo(Action& action,IntPair<As,Bs>...);

protected:
	void setInputs(StaticInput<Inputs>&... inputs){
		inputSlots = std::tuple<StaticInput<Inputs>*...>{&inputs...};
	}

	void setOutputs(StaticOutput<Outputs>&... outputs){
		outputSlots = std::tuple<StaticOutput<Outputs>*...>{&outputs...};
	}
private:
	std::tuple<StaticInput<Inputs>*...> inputSlots;
	std::tuple<StaticOutput<Outputs>*...> outputSlots;
};

template<typename... Inputs,typename... Outputs>
template<typename Action,int... As,int... Bs>
void BoxedAction<Input(Inputs...),Output(Outputs...)>::connectTo(Action& action,IntPair<As,Bs>...){
	variadicForEach(this->getOutput<As>().connectTo(action.getInput<Bs>()));
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& BoxedAction<Input(Inputs...), Output(Outputs...)>::getInput(){
	return *std::get<N>(inputSlots);
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& BoxedAction<Input(Inputs...), Output(Outputs...)>::getOutput(){
	return *std::get<N>(outputSlots);
}
