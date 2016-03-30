#pragma once
#include "../Pipeline/StaticPipelineAction.h"

/*
 * LambdaAction.h
 *
 *  Created on: 30.03.2016
 *      Author: tly
 */

template<typename...> struct FunctionAction;

template<typename... Inputs,typename... Outputs>
struct FunctionAction<Input(Inputs...),Output(Outputs...)>
	: StaticPipelineAction<Input(Inputs...),Output(Outputs...)>{

	FunctionAction(std::function<void(FunctionAction<Input(Inputs...),Output(Outputs...)>&)> func)
		:generatorFunc(func){}

	virtual ~FunctionAction() = default;
protected:
	virtual void execute(){
		generatorFunc(*this);
	}

	std::function<std::tuple<Outputs...>(Inputs...)> generatorFunc;
};



