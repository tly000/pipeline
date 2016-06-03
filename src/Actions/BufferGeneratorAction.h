#pragma once
#include "../Actions/LazyAction.h"
#include "../Type/Range.h"

/*
 * BufferGeneratorAction.h
 *
 *  Created on: 03.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct BufferGeneratorAction
	: LazyAction<Input(KV("elemCount", uint32_t)),Output(typename Factory::template Buffer<T>)>{

	BufferGeneratorAction(Factory factory) : factory(factory){}
protected:
	Factory factory;

	void executeImpl(){
		uint32_t elemCount = this->template getInput<0>().getValue();
		this->template getOutput<0>().setValue(factory.template createBuffer<Val<T>>(elemCount));
	}
};
