#pragma once
/*
 * Complex.h
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

#include "../MacroTools.h"
#include "StandardTypes.h"
#ifndef Type
#define Type float
#endif

#define __HEADER(x) #x
#define _HEADER(x) __HEADER(x.h)
#define HEADER(x) _HEADER(x)
#include HEADER(Type)

#define floatToType _concat(floatTo,Type)

typedef struct{
	Type real,imag;
} Complex;

inline Type cabs2(const Complex a){
	return tadd(tmul(a.real,a.real),tmul(a.imag,a.imag));
}
inline float cfabs2(const Complex a){
	//float r = tofloat(a.real);
	//float i = tofloat(a.imag);
	//return r*r + i*i;
	return tofloat(cabs2(a));
}

inline Complex cadd(const Complex a,const Complex b){
	return (Complex) {
		tadd(a.real,b.real),
		tadd(a.imag,b.imag)
	};
}

inline Complex csub(const Complex a,const Complex b){
	return (Complex) {
		tsub(a.real,b.real),
		tsub(a.imag,b.imag)
	};
}

inline Complex cmul(const Complex a,const Complex b){
	return (Complex) {
		tsub(tmul(a.real,b.real),tmul(a.imag,b.imag)),
		tadd(tmul(a.imag,b.real),tmul(a.real,b.imag))
	};
}

inline Complex csqr(const Complex a){
	return (Complex) {
		tsub(tmul(a.real,a.real),tmul(a.imag,a.imag)),
		tmul(floatToType(2),tmul(a.real,a.imag))
	};
}

inline Complex cdiv(const Complex a,const Complex b){
	Type abs2 = cabs2(b);
	return (Complex) {
		tdiv(tadd(tmul(a.real,b.real),tmul(a.imag,b.imag)),abs2),
		tdiv(tsub(tmul(a.imag,b.real),tmul(a.real,b.imag)), abs2)
	};
}




