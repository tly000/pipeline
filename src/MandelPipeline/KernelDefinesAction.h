#pragma once
#include "../Actions/FunctionAction.h"

/*
 * KernelDefinesAction.h
 *
 *  Created on: 05.05.2016
 *      Author: tly
 */

template<typename T> struct KernelDefinesActionImpl;

template<typename T,size_t I> using JustT = T;
template<size_t... I> struct KernelDefinesActionImpl<std::index_sequence<I...>>
	: LazyAction<Input(JustT<std::string,I>...),Output(std::string)>{

	template<typename... Names> KernelDefinesActionImpl(Names... names)
	: names{names...}{
		if(sizeof...(Names) != sizeof...(I)){
			throw std::runtime_error("KernelDefinesAction needs a definition name for every input");
		}
	}
protected:
	std::string names[sizeof...(I)];

	void executeImpl(){
		std::string params = "";
		variadicForEach(params += "-D" + names[I] + "=" + this->template getInput<I>().getValue() + " ");
		this->template getOutput<0>().setValue(params);
	}
};


template<size_t N> using KernelDefinesAction = KernelDefinesActionImpl<std::make_index_sequence<N>>;

