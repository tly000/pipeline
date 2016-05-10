#pragma once
#include "../Actions/FunctionAction.h"
#include "../Type/TypeHelper.h"

/*
 * ToStringAction.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

template<typename... Types> struct ToStringAction
	: FunctionAction<Input(Types...),Output(Just<std::string,Types>...)>{

	ToStringAction()
		:FunctionAction<Input(Types...),Output(Just<std::string,Types>...)>{
			&toString<Types>...
		}{}
};


