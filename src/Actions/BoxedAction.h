#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * BoxedAction.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

template<typename...> struct BoxedAction;

template<typename... Inputs,typename... Outputs> struct BoxedAction
  : StaticPipelineAction<Input(Inputs...),Output(Outputs...)>{

protected:
	template<size_t N> void delegateInput(decltype(this->getInput<N>())& inputSlot){
		inputDelegation.at(N) = [&]{
			inputSlot.setDefaultValue(this->template getInput<N>().getValue());
		};
	}

	template<size_t N> void delegateOutput(decltype(this->getOutput<N>())& outputSlot){
		outputDelegation.at(N) = [&]{
			this->template getOutput<N>().setValue(outputSlot.getValue());
		};
	}

	void execute(){
		for(auto& p : inputDelegation){
			p.second();
		}
		this->executeImpl();
		for(auto& p : outputDelegation){
			p.second();
		}
	}

	virtual void executeImpl() = 0;

	virtual ~BoxedAction() = default;
private:
	std::map<int,std::function<void()>> inputDelegation;
	std::map<int,std::function<void()>> outputDelegation;
};


