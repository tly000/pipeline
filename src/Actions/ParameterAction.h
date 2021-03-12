#pragma once
#include "../Pipeline/StaticPipelineAction.h"
#include <map>

/*
 * ParameterAction.h
 *
 *  Created on: 30.03.2016
 *      Author: tly
 */

template<typename... Types>
struct ParameterAction : StaticPipelineAction<Input(),Output(Types...)>{

	template<size_t N = 0,typename T> void setValue(const T& val){
		this->template getOutput<N>().setValue(val);
	}
	template<size_t N = 0> const auto& getValue(){
		return this->template getOutput<N>().getValue();
	}

	template<typename S,typename T> void setValue(const S& s,const T& val){
		this->getOutput(s).setValue(val);
	}
	template<typename S> const auto& getValue(const S& s){
		return this->getOutput(s).getValue();
	}
protected:
	void execute(){};
};


