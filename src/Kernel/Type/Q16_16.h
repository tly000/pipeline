#pragma once
/*
 * Q16_16.h
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

#include "../Type/StandardTypes.h"

typedef int32_t Q16_16;

inline Q16_16 tadd(const Q16_16 a,const Q16_16 b){
	return a + b;
}

inline Q16_16 tsub(const Q16_16 a,const Q16_16 b){
	return a - b;
}

inline Q16_16 tmul(const Q16_16 a,const Q16_16 b){
	return ((int64_t)(a)*(int64_t)(b)) >> 16;
	//fast: (a >> 8) * (b >> 8)
}

inline Q16_16 tdiv(const Q16_16 a,const Q16_16 b){
	return ((int64_t)(a << 16)) / b;
}

inline Q16_16 tabs(const Q16_16 a){
	return	abs(a);
}

inline float tofloat(const Q16_16 a){
	return	(float)(a) / (1 << 16);
}

inline Q16_16 floatToQ16_16(const float a){
	return	a * (1 << 16);
}

