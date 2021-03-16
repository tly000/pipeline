#pragma once
/*
 * Float.h
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

inline float tadd(const float a,const float b){
	return a + b;
}

inline float tsub(const float a,const float b){
	return a - b;
}

inline float tmul(const float a,const float b){
	return a * b;
}

inline float tdiv(const float a,const float b){
	return a / b;
}

inline float tofloat(const float a){
	return a;
}

inline float floatTofloat(const float a){
	return a;
}

inline float tabs(float a){
	return fabs(a);
}

inline float tsqrt(float a){
	return sqrt(a);
}

inline float tneg(float a){
	return -a;
}

#ifdef __OPENCL_VERSION__

inline float tpow(float a,int b){
	return pown(a,b);
}

#else
#include <cmath>

inline float tpow(float a,int b){
	return std::pow(a,b);
}

#endif
