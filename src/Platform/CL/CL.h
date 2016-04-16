#pragma once

/*
 * CL.h
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

#define __CL_ENABLE_EXCEPTIONS
#ifdef __GNUC__
	//remove compiler warnings in cl.hpp

	#pragma GCC diagnostic push
	#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
	#include <CL/cl.hpp>
	#pragma GCC diagnostic pop

#else
	#include <CL/cl.hpp>
#endif





