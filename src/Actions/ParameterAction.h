#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * ParameterAction.h
 *
 *  Created on: 30.03.2016
 *      Author: tly
 */

template<typename T>
struct ParameterAction : StaticPipelineAction<Input(),Output(T)>{
	void setValue(const T& val){
		this->template getOutput<0>().setValue(val);
	}
	const T& getValue() const{
		return this->template getOutput<0>().getValue();
	}
protected:
	void execute(){};
};


