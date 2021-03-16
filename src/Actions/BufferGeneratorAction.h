#pragma once
#include "../Actions/LazyAction.h"
#include "../Type/Range.h"
#include "../Platform/Factory.h"
#include <vector>

/*
 * BufferGeneratorAction.h
 *
 *  Created on: 03.06.2016
 *      Author: tly
 */

template<typename T> struct BufferGeneratorAction
	: LazyAction<Input(KV("elemCount", uint32_t)),Output(Buffer<T>)>{

	BufferGeneratorAction(Factory& factory) : factory(factory){}
protected:
	Factory& factory;

	void executeImpl(){
		uint32_t elemCount = this->template getInput<0>().getValue();
		auto buffer = Buffer<T>(factory.createBuffer(elemCount, sizeof(T)));
		this->template getOutput<0>().setValue(buffer);
	}
};

template<typename T> struct FilledBufferGeneratorAction
	: LazyAction<Input(KV("data", std::vector<T>)),Output(Buffer<T>)>{

	FilledBufferGeneratorAction(Factory& factory) : factory(factory){}
protected:
	Factory& factory;

	void executeImpl(){
		auto& data = this->getInput(_C("data")).getValue();
		auto buffer = Buffer<T>(factory.createBuffer(data.size(), sizeof(T)));
		buffer.copyFromBuffer(data.data(), data.data() + data.size());
		this->template getOutput<0>().setValue(buffer);
	}
};
