#pragma once
/*
 * Float.h
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

#ifdef __OPENCL_VERSION___
	#ifdef cl_khr_fp64
		#pragma OPENCL EXTENSION cl_khr_fp64 : enable
	#else
		#error "double precision is not supported on this device."
	#endif
#endif

inline double tadd(const double a,const double b){
	return a + b;
}

inline double tsub(const double a,const double b){
	return a - b;
}

inline double tmul(const double a,const double b){
	return a * b;
}

inline double tdiv(const double a,const double b){
	return a / b;
}

inline double tofloat(const double a){
	return a;
}

inline double floatTodouble(const float a){
	return a;
}

inline double tabs(const double a){
	return fabs(a);
}

inline double tsqrt(const double a){
	return sqrt(a);
}

inline double tneg(const double a){
	return -a;
}

#ifdef __OPENCL_VERSION__

inline double tpow(double a,int b){
	return pown(a,b);
}

#else
#include <cmath>

inline double tpow(double a,int b){
	return std::pow(a,b);
}
#endif

