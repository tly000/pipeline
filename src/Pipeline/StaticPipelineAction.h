#pragma once
#include <tuple>
#include <cstdint>
#include <set>
#include <memory>
#include "AbstractPipelineAction.h"
#include "../Utility/NonCopyable.h"
#include "../Utility/VariadicUtils.h"
#include "StaticInputOutput.h"
#include "../Type/MapStruct.h"

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

template<typename T> struct UnpackType{
	using key_type = decltype("unnamed"_c);
	using value_type = T;
};

template<typename A,typename B> struct UnpackType<KeyValuePair<A,B>>{
	using key_type = A;
	using value_type = B;
};

template<typename... Inputs,typename... Outputs>
struct StaticPipelineAction<Input(Inputs...),Output(Outputs...)> : AbstractPipelineAction, NonCopyable{
	StaticPipelineAction()
		:inputSlots{std::make_unique<StaticInput<typename UnpackType<Inputs>::value_type>>(this,UnpackType<Inputs>::key_type::toString())...}//,
		 //outputSlots{std::make_unique<StaticOutput<typename UnpackType<Outputs>::value_type>>(this,UnpackType<Outputs>::key_type::toString())...}
		 {}

	template<size_t N> StaticInput<NthType<N,Inputs...>>& getInput(){
		return *std::get<N>(inputSlots);
	}
	template<size_t N> StaticOutput<NthType<N,Outputs...>>& getOutput(){
		return *std::get<N>(outputSlots);
	}

	template<typename String> auto& getInput(const String&){
		using Index = IndexOf<0,String,typename UnpackType<Inputs>::key_type...>;
		static_assert(Index::value != -1, "Input not found.");
		return this->getInput<Index::value>();
	}
	template<typename String> auto& getOutput(const String&){
		using Index = IndexOf<0,String,typename UnpackType<Outputs>::key_type...>;
		static_assert(Index::value != -1, "Output not found.");
		return this->getOutput<Index::value>();
	}

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

