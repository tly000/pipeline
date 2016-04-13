#pragma once
#include "../Pipeline/StaticPipelineAction.h"

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
	template<size_t N = 0>  const auto& getValue() const{
		return this->template getOutput<N>().getValue();
	}
protected:
	void execute(){};
};


