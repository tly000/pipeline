#pragma once

/*
 * float128.h
 *
 *  Created on: 27.05.2016
 *      Author: tly
 */

#ifdef __cplusplus
#include <quadmath.h>

using float128 = __float128;

inline float128 tadd(const float128 a,const float128 b){
	return a + b;
}

inline float128 tsub(const float128 a,const float128 b){
	return a - b;
}

inline float128 tmul(const float128 a,const float128 b){
	return a * b;
}

inline float128 tdiv(const float128 a,const float128 b){
	return a / b;
}

inline float tofloat(const float128 a){
	return a;
}

inline float128 floatTofloat128(const float a){
	return a;
}

inline float128 tabs(float128 a){
	return fabsq(a);
}

inline float128 tsqrt(float128 a){
	return sqrtq(a);
}

inline float128 tneg(float128 a){
	return -a;
}

inline float128 tpow(float128 a,int b){
	float128 r = a;
	for(int i = 1; i < b; i++){
		r *= a;
	}
	return r;
}

#else
#error "float128 only supported on c++"
#endif

