#pragma once
#include "CL.h"
#include "../Image.h"
#include "../../Type/Vec.h"

/*
 * CLImage.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct CLImage : Image<T>, CLBuffer<T>{
	CLImage(cl::Context& ctx,cl::CommandQueue queue,cl_mem_flags memFlags,size_t width,size_t height)
		:Image<T>(width,height),
		 CLBuffer<T>(ctx,queue,memFlags,width * height){}
};
