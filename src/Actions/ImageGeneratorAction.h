#pragma once
#include "../Actions/LazyAction.h"
#include "../Platform/Factory.h"
#include "../Type/Range.h"

/*
 * ImageGeneratorAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename T> struct ImageGeneratorAction
	: LazyAction<Input(KV("imageRange",Range)),Output(Image<T>)>{

	ImageGeneratorAction(Factory& factory) : factory(factory){}
protected:
	Factory& factory;

	void executeImpl(){
		uint32_t width = this->template getInput<0>().getValue().x;
		uint32_t height = this->template getInput<0>().getValue().y;
		this->template getOutput<0>().setValue(Image<T>(width, height, factory.createBuffer(width * height, sizeof(T))));
	}
};


