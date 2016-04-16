#pragma once
#include "../Pipeline/StaticPipelineAction.h"
#include "../Type/Range.h"
#include <tuple>

/*
 * KernelAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<size_t...> struct KernelOutput{};

template<typename...> struct KernelAction;

template<typename Factory,
		 typename... Inputs,
		 size_t... Outputs>
struct KernelAction<Factory,Input(Inputs...),KernelOutput<Outputs...>>
	: StaticPipelineAction<Input(typename Factory::template Kernel<Inputs...>,Range,Range,Range,Inputs...),Output(NthType<Outputs,Inputs...>...)>{

	KernelAction(){
		this->getLocalSizeInput().setDefaultValue(Range{1,1,1});
		this->getGlobalOffsetInput().setDefaultValue(Range{0,0,0});
	}

	virtual ~KernelAction() = default;

	virtual void execute(){
		this->executeImpl(std::make_index_sequence<3 + sizeof...(Inputs)>());
	}

	using KernelType = typename Factory::template Kernel<Inputs...>;

	StaticInput<KernelType>& getKernelInput(){
		return this->template getInput<0>();
	}

	StaticInput<Range>& getGlobalOffsetInput(){
		return this->template getInput<1>();
	}
	StaticInput<Range>& getGlobalSizeInput(){
		return this->template getInput<2>();
	}
	StaticInput<Range>& getLocalSizeInput(){
		return this->template getInput<3>();
	}
private:
	template<size_t... N> void executeImpl(std::index_sequence<N...>){
		auto& kernel = this->template getInput<0>().getValue();

		//run kernel
		kernel.run(this->template getInput<N+1>().getValue()...);
		//delegate inputs to outputs
		this->delegateOutputs(std::make_index_sequence<sizeof...(Outputs)>());
	}

	template<size_t... N> void delegateOutputs(std::index_sequence<N...>){
		variadicForEach(this->template getOutput<N>().setValue(
			this->template getInput<Outputs + 4>().getValue()
		))
	}
};


