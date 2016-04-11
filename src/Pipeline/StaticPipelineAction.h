#pragma once
#include <tuple>
#include <cstdint>
#include <set>
#include <memory>
#include "AbstractPipelineAction.h"
#include "../Utility/NonCopyable.h"
#include "../Utility/VariadicUtils.h"
#include "StaticInputOutput.h"

/*
 * PipelineAction.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

struct Input{};
struct Output{};
template<int A,int B> struct IntPair{};

template<typename...> struct StaticPipelineAction;

template<typename... Inputs,typename... Outputs>
struct StaticPipelineAction<Input(Inputs...),Output(Outputs...)> : AbstractPipelineAction, NonCopyable{
	StaticPipelineAction()
		:inputSlots{std::make_unique<StaticInput<Inputs>>(this)...},
		 outputSlots{std::make_unique<StaticOutput<Outputs>>(this)...}{}

	template<size_t N> auto& getInput();
	template<size_t N> auto& getOutput();

	template<typename Action,int... As,int... Bs>
	void connectTo(Action& action,IntPair<As,Bs>...);

	virtual ~StaticPipelineAction() = default;

	const static uint32_t numInputs = sizeof...(Inputs);
	const static uint32_t numOutputs = sizeof...(Outputs);
protected:
	std::tuple<std::unique_ptr<StaticInput<Inputs>>...> inputSlots;
	std::tuple<std::unique_ptr<StaticOutput<Outputs>>...> outputSlots;
};

template<typename... Inputs,typename... Outputs>
template<typename Action,int... As,int... Bs>
void StaticPipelineAction<Input(Inputs...),Output(Outputs...)>::connectTo(Action& action,IntPair<As,Bs>...){
	variadicForEach(this->getOutput<As>().connectTo(action.getInput<Bs>()));
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& StaticPipelineAction<Input(Inputs...), Output(Outputs...)>::getInput(){
	return *std::get<N>(inputSlots);
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& StaticPipelineAction<Input(Inputs...), Output(Outputs...)>::getOutput(){
	return *std::get<N>(outputSlots);
}
