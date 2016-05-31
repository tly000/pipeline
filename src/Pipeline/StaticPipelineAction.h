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

template<typename T> using Val = typename UnpackType<T>::value_type;
template<typename T> using Key = typename UnpackType<T>::key_type;

template<typename... Inputs,typename... Outputs>
struct StaticPipelineAction<Input(Inputs...),Output(Outputs...)> : AbstractPipelineAction, NonCopyable{
	StaticPipelineAction()
		:inputSlots{std::make_unique<StaticInput<Val<Inputs>>(this,Key<Inputs>::toString())...},
		 outputSlots{std::make_unique<StaticOutput<Val<Outputs>>(this,Key<Outputs>::toString())...}{}

	template<size_t N> auto& getInput(){
		return *std::get<N>(inputSlots);
	}
	template<size_t N> auto& getOutput(){
		return *std::get<N>(outputSlots);
	}

	template<typename String> auto& getInput(const String&){
		using Index = IndexOf<0,String,Key<Inputs>...>;
		static_assert(Index::value != -1, "Input not found.");
		return this->getInput<Index::value>();
	}
	template<typename String> auto& getOutput(const String&){
		using Index = IndexOf<0,String,Key<Outputs>...>;
		static_assert(Index::value != -1, "Output not found.");
		return this->getOutput<Index::value>();
	}

	virtual ~StaticPipelineAction() = default;

	const static uint32_t numInputs = sizeof...(Inputs);
	const static uint32_t numOutputs = sizeof...(Outputs);

	template<int... Indices> auto output(){
		return std::make_tuple(
			&this->template getOutput<Indices>()...
		);
	}

	template<int... Indices> auto input(){
		return std::make_tuple(
			&this->template getInput<Indices>()...
		);
	}
protected:
	std::tuple<std::unique_ptr<StaticInput<Val<Inputs>>>...> inputSlots;
	std::tuple<std::unique_ptr<StaticOutput<Val<Outputs>>>...> outputSlots;
};

template<typename... T> void operator>>(
	std::tuple<StaticOutput<T>*...> outputs,
	std::tuple<StaticInput<T>*...> inputs){
	connectToImpl(outputs,inputs,std::index_sequence_for<T...>());
}

template<typename... T,size_t... I> void connectToImpl(
	std::tuple<StaticOutput<T>*...> outputs,
	std::tuple<StaticInput<T>*...> inputs,std::index_sequence<I...>){
	variadicForEach(std::get<I>(outputs)->connectTo(*std::get<I>(inputs)));
}

template<typename T> void operator>>(
	std::tuple<StaticOutput<T>*> outputs,
	StaticInput<T>& input){
	std::get<0>(outputs)->connectTo(input);
}

template<typename T> void operator>>(
	StaticOutput<T>& output,
	StaticInput<T>& input){
	output.connectTo(input);
}


