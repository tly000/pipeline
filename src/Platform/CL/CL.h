#pragma once

/*
 * CL.h
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_MINIMUM_OPENCL_VERSION 120
#define CL_HPP_TARGET_OPENCL_VERSION 120
#ifdef __GNUC__
	//remove compiler warnings in cl.hpp

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	#include <CL/cl.hpp>
	#pragma GCC diagnostic pop

#else
	#include <CL/cl2.hpp>
#endif





