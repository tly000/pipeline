#pragma once
#include "../Image.h"
#include "CPUBuffer.h"
#include <memory>

/*
 * CPUImage.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct CPUImage : CPUBuffer<T>, Image<T>{
	CPUImage(size_t width,size_t height)
		:CPUBuffer<T>(width*height),
		 Image<T>(width,height){}

	T& at(size_t x,size_t y){
		if(x < this->width && y < this->height && x >= 0 && y >= 0){
			return this->getDataPointer()[y * this->width + x];
		} else{
			throw std::runtime_error("index out of bounds.");
		}
	}

	const T& at(size_t x,size_t y) const {
		if(x < this->width && y < this->height && x >= 0 && y >= 0){
			return this->getDataPointer()[y * this->width + x];
		} else{
			throw std::runtime_error("index out of bounds.");
		}
	}
};



