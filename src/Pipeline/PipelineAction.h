#pragma once
#include <tuple>
#include <cstdint>
#include <set>
#include "AbstractPipelineAction.h"
#include "InputOutputSlots.h"
#include "../Utility/NonCopyable.h"
#include "../Utility/VariadicUtils.h"

/*
 * PipelineAction.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

struct Input{};
struct Output{};
template<int A,int B> struct IntPair{};

template<typename...> struct PipelineAction;

template<typename... Inputs,typename... Outputs>
struct PipelineAction<Input(Inputs...),Output(Outputs...)> : AbstractPipelineAction, NonCopyable{
	PipelineAction()
		:inputSlots{InputSlot<Inputs>(this)...},
		 outputSlots{OutputSlot<Outputs>(this)...}{}

	virtual std::set<AbstractPipelineAction*> getParentActions();

	template<size_t N> auto& getInput();
	template<size_t N> auto& getOutput();

	template<typename Action,int... As,int... Bs>
	void connectTo(Action& action,IntPair<As,Bs>...);

	virtual ~PipelineAction() = default;
protected:
	std::tuple<InputSlot<Inputs>...> inputSlots;
	std::tuple<OutputSlot<Outputs>...> outputSlots;
};

template<typename... Inputs,typename... Outputs>
template<typename Action,int... As,int... Bs>
void PipelineAction<Input(Inputs...),Output(Outputs...)>::connectTo(Action& action,IntPair<As,Bs>...){
	variadicForEach(this->getOutput<As>().connectTo(action.getInput<Bs>()));
}

template<typename... Inputs, typename... Outputs>
std::set<AbstractPipelineAction*> PipelineAction<Input(Inputs...), Output(Outputs...)>::getParentActions(){
	std::set<AbstractPipelineAction*> parents;
	tupleForEach(inputSlots,[&](auto& inputSlot){
		if(auto* outputSlot = inputSlot.getOutputSlot()){
			parents.insert(outputSlot->getPipeline());
		}
	});
	return parents;
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& PipelineAction<Input(Inputs...), Output(Outputs...)>::getInput(){
	return std::get<N>(inputSlots);
}

template<typename... Inputs, typename... Outputs> template<size_t N>
auto& PipelineAction<Input(Inputs...), Output(Outputs...)>::getOutput(){
	return std::get<N>(outputSlots);
}
