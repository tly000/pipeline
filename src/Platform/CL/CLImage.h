#pragma once
#include "../Image.h"
#include <CL/cl.hpp>

/*
 * CLImage.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct CLChannelOrder;
template<typename T> struct CLChannelDataType;

template<typename T> struct CLImageFormat{
	const static cl::ImageFormat value;
};

template<typename T> const cl::ImageFormat CLImageFormat<T>::value = {
	CLChannelOrder<T>::value,
	CLChannelDataType<T>::value
};

template<typename T> struct CLImage : Image<T>{
	CLImage(cl::Context& ctx,cl_mem_flags memFlags,size_t width,size_t height)
		:Image<T>(width,height),
		 imageHandle(ctx,memFlags,CLImageFormat<T>::value,width,height){}
protected:
	cl::Image2D imageHandle;
};

//TODO CLImageOrder and CLImageDataType specializations

