#pragma once
/*
 * Complex.h
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

#ifndef Type
#define Type float
#include "Float.h"
#endif

typedef struct{
	Type real,imag;
} Complex;

inline Type cabs2(Complex a){
	return tadd(tmul(a.real,a.real),tmul(a.imag,a.imag));
}

inline Complex cadd(Complex a,Complex b){
	return {
		tadd(a.real,b.real),
		tadd(a.imag,b.imag)
	};
}

inline Complex csub(Complex a,Complex b){
	return {
		tsub(a.real,b.real),
		tsub(a.imag,b.imag)
	};
}

inline Complex cmul(Complex a,Complex b){
	return {
		tsub(tmul(a.real,b.real),tmul(a.imag,b.imag)),
		tadd(tmul(a.imag,b.real),tmul(a.real,b.imag))
	};
}

inline Complex cdiv(Complex a,Complex b){
	Type abs2 = cabs2(b);
	return {
		tdiv(tadd(tmul(a.real,b.real),tmul(a.imag,b.imag)),abs2),
		tdiv(tsub(tmul(a.imag,b.real),tmul(a.real,b.imag)), abs2)
	};
}





