#pragma once
#include "../Pipeline/StaticPipelineAction.h"
#include <functional>
#include <map>

/*
 * BoxedAction.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

template<typename...> struct BoxedAction;

template<typename... Inputs,typename... Outputs> struct BoxedAction<Input(Inputs...),Output(Outputs...)>
  : StaticPipelineAction<Input(Inputs...),Output(Outputs...)>{

protected:
	template<size_t N> void delegateInput(StaticInput<NthType<N,Val<Inputs>...>>& inputSlot){
		inputDelegation.emplace(N,[&]{
			inputSlot.setDefaultValue(this->template getInput<N>().getValue());
		});
	}

	template<size_t N> void delegateOutput(StaticOutput<NthType<N,Val<Outputs>...>>& outputSlot){
		outputDelegation.emplace(N,[&]{
			this->template getOutput<N>().setValue(outputSlot.getValue());
		});
	}

	template<typename String,typename T> void delegateInput(const String&, StaticInput<T>& inputSlot){
		using Index = IndexOf<String,Key<Inputs>...>;
		static_assert(Index::value != -1, "Input not found.");
		inputDelegation.emplace(Index::value,[&]{
			inputSlot.setDefaultValue(this->getInput(String()).getValue());
		});
	}

	template<typename String,typename T> void delegateOutput(const String&,StaticOutput<T>& outputSlot){
		using Index = IndexOf<String,Key<Outputs>...>;
		static_assert(Index::value != -1, "Output not found.");
		outputDelegation.emplace(Index::value,[&]{
			this->getOutput(String()).setValue(outputSlot.getValue());
		});
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
	std::multimap<int,std::function<void()>> inputDelegation;
	std::map<int,std::function<void()>> outputDelegation;
};


