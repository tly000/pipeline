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

template<typename T> struct CLChannelOrder{
	const static cl_channel_order value = CL_R;
};
template<typename T> struct CLChannelDataType{};

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

	cl::Image2D getHandle() const {
		return imageHandle;
	}
protected:
	cl::Image2D imageHandle;
};

template<unsigned N,typename T> struct CLChannelOrder<Vec<N,T>>{
	const static cl_channel_order value =
		N == 1 ? CL_R :
		N == 2 ? CL_RG :
		N == 3 ? CL_RGB :
		N == 4 ? CL_RGBA : 0;
	static_assert(N <= 4,"CLChannelOrder only allows up to 4 channels");
};

template<unsigned N,typename T> struct CLChannelDataType<Vec<N,T>>{
	const static cl_channel_type value = CLChannelDataType<T>::value;
};

template<> struct CLChannelDataType<uint32_t>{
	const static cl_channel_type value = CL_UNSIGNED_INT32;
};

template<> struct CLChannelDataType<float>{
	const static cl_channel_type value = CL_FLOAT;
};

