#pragma once
#include "../Actions/LazyAction.h"
#include "../Type/Range.h"
#include <vector>

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
		auto buffer = factory.template createBuffer<Val<T>>(elemCount);
		this->template getOutput<0>().setValue(buffer);
	}
};

template<typename Factory,typename T> struct FilledBufferGeneratorAction
	: LazyAction<Input(KV("data", std::vector<T>)),Output(typename Factory::template Buffer<T>)>{

	FilledBufferGeneratorAction(Factory factory) : factory(factory){}
protected:
	Factory factory;

	void executeImpl(){
		auto& data = this->template getInput("data"_c).getValue();
		auto buffer = factory.template createBuffer<Val<T>>(data.size());
		buffer.copyFromBuffer(data,0,data.size());
		this->template getOutput<0>().setValue(buffer);
	}
};
