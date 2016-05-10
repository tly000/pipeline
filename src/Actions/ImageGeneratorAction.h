#pragma once
#include "../Actions/LazyAction.h"
#include "../Type/Range.h"

/*
 * ImageGeneratorAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct ImageGeneratorAction
	: LazyAction<Input(Range),Output(typename Factory::template Image<T>)>{

	ImageGeneratorAction(Factory factory) : factory(factory){}
protected:
	Factory factory;

	void executeImpl(){
		uint32_t width = this->template getInput<0>().getValue().x;
		uint32_t height = this->template getInput<0>().getValue().y;
		this->template getOutput<0>().setValue(factory.template createImage<T>(width,height));
	}
};


