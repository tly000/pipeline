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
	: LazyAction<Input(KV("imageRange",Range), KV("platform", std::shared_ptr<Factory>)),Output(Image<T>)>{
protected:
	void executeImpl(){
		uint32_t width = this->template getInput<0>().getValue().x;
		uint32_t height = this->template getInput<0>().getValue().y;
        auto& factory = *this->getInput(_C("platform")).getValue();
        this->template getOutput<0>().setValue(Image<T>(factory.createImage(width, height, sizeof(T))));
	}
};


