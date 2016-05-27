#pragma once

/*
 * longdouble.h
 *
 *  Created on: 27.05.2016
 *      Author: tly
 */

using longdouble = long double;

inline longdouble tadd(const longdouble a,const longdouble b){
	return a + b;
}

inline longdouble tsub(const longdouble a,const longdouble b){
	return a - b;
}

inline longdouble tmul(const longdouble a,const longdouble b){
	return a * b;
}

inline longdouble tdiv(const longdouble a,const longdouble b){
	return a / b;
}

inline float tofloat(const longdouble a){
	return a;
}

inline longdouble floatTolongdouble(const float a){
	return a;
}

inline longdouble tabs(longdouble a){
	return fabsl(a);
}

inline longdouble tsqrt(longdouble a){
	return sqrtl(a);
}

inline longdouble tneg(longdouble a){
	return -a;
}

inline longdouble tpow(longdouble a,int b){
	longdouble r = a;
	for(int i = 1; i < b; i++){
		r *= a;
	}
	return r;
}


