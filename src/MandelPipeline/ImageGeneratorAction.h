#pragma once
#include "../Actions/LazyAction.h"

/*
 * ImageGeneratorAction.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct ImageGeneratorAction
	: LazyAction<Input(uint32_t,uint32_t),Output(typename Factory::template Image<T>)>{

	ImageGeneratorAction(Factory factory) : factory(factory){}
protected:
	Factory factory;

	void executeImpl(){
		uint32_t width = this->template getInput<0>().getValue();
		uint32_t height = this->template getInput<1>().getValue();
		this->template getOutput<0>().setValue(factory.template createImage<T>(width,height));
	}
};


