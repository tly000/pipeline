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
	return ((int64_t)(a) * (int64_t)(b)) >> 16;
}

inline Q16_16 tdiv(const Q16_16 a,const Q16_16 b){
	return ((int64_t)(a << 16)) / b;
}

inline float tofloat(const Q16_16 a){
	return	(float)(a) / (1 << 16);
}

inline Q16_16 fromfloat(const float a){
	return	a * (1 << 16);
}

