#pragma once
#include "LazyAction.h"
#include "../Type/TypeHelper.h"

/*
 * KernelDefinesAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

template<typename...> struct KernelDefinesAction;

template<typename... Inputs,typename... Strings> struct KernelDefinesAction<KeyValuePair<Strings,Inputs>...>
  : LazyAction<Input(KeyValuePair<Strings,Inputs>...),Output(KV("defines",std::string))>{

protected:
	void executeImpl(){
		std::string params = "";
		variadicForEach(
			params += "-D" + Strings::toString() + "=\"" + toString(this->getInput(Strings()).getValue()) + "\" "
		);
		this->getOutput(_C("defines")).setValue(params);
	}
};



