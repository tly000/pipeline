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

template<typename T> struct CPUImage : Image<T>, CPUBuffer<T>{
	CPUImage(size_t width,size_t height)
		:CPUBuffer(width*height),
		 Image(width,height){}
};



