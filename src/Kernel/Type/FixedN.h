#pragma once

/*
 * fixedPointN.h
 *
 *  Created on: 22.04.2016
 *      Author: tly
 */

#include "StandardTypes.h"
#ifndef FixedSize
#define FixedSize 8
#endif

#ifdef __OPENCL_VERSION__

#else
#include <math.h>
#endif

typedef struct{
	int32_t sign;
	uint32_t words[FixedSize];
} Fixed;

inline Fixed fixedAdd(const Fixed a,const Fixed b){
	Fixed c = {a.sign}; //just copy the sign

	uint64_t carry = 0;
	for(int i =FixedSize-1; i >= 0; i--){
		carry += (uint64_t)(a.words[i]) + (uint64_t)(b.words[i]);
		c.words[i] = carry & 0xFFFFFFFF;
		carry >>= 32;
	}
	return c;
}

inline Fixed fixedSub(const Fixed a,const Fixed b){
	Fixed c = {a.sign}; //just copy the sign

	uint64_t carry = 0;
	for(int i =FixedSize-1; i >= 0; i--){
		carry = (uint64_t)(a.words[i]) - (uint64_t)(b.words[i]) - carry;
		c.words[i] = carry & 0xFFFFFFFF;
		//carry = (carry >= 0x10000000 ? 1 : 0);
		carry = (carry >> 32) & 1;
	}
	if(carry){
		//there was an overflow.
		//=>_Need to invert the sign and the most significant word.
		std::cout << std::hex << c.words[0] << "," << c.words[1] << ","<< c.words[2] << std::endl;
		c.sign *= -1;
		c.words[0] = ~c.words[0] -1;
	}
	return c;
}

inline Fixed tadd(const Fixed a,const Fixed b){
	if(a.sign == b.sign){
		return fixedAdd(a,b);
	}else{
		return fixedSub(a,b);
	}
}


inline Fixed tsub(const Fixed a,const Fixed b){
	if(a.sign == b.sign){
		return fixedSub(a,b);
	}else{
		return fixedAdd(a,b);
	}
}

inline Fixed tmul(const Fixed a,const Fixed b){
	Fixed c;
	c.sign = a.sign * b.sign;
	uint64_t carry[FixedSize];

	for(int i = 0; i <FixedSize; i++){
		c.words[i] = 0;
		carry[i] = 0;
	}

	for(int i =FixedSize-1; i >= 0; i--){
		for(int j =FixedSize-1; j >= 0; j--){
			uint16_t k = i + j;
			uint64_t r = (uint64_t)(a.words[i]) * (uint64_t)(b.words[j]);
			if(k <= FixedSize){
				if(k < FixedSize) carry[k] += (uint32_t)(r & 0xFFFFFFFF);
				if(k > 0) carry[k-1] += (uint32_t)(r >> 32);
			}
		}
	}

    uint64_t currentCarry = 0;
	for (int i = FixedSize - 1; i >= 0; i--) {
		currentCarry += carry[i];
		c.words[i] = (uint32_t)(currentCarry & 0xFFFFFFFF);
		currentCarry >>= 32;
	}
	return c;
}

inline Fixed fromfloat(float a){
	Fixed f = {
		a < 0 ? -1 : 1
	};
	a = fabs(a);
	f.words[0] = (uint32_t)(a);
	f.words[1] = (uint32_t)(fmod(a,1) * ((uint64_t)(1) << 32));
	for(int i = 2; i <FixedSize; i++){
		f.words[i] = 0;
	}
	return f;
}

inline Fixed tdiv(const Fixed a,const Fixed b){
	return (Fixed){};
}

inline float tofloat(const Fixed a){
	return	a.sign * (
		(float)(a.words[0]) +
		(float)((double)(a.words[1]) / ((uint64_t)(1) << 32))
	);
}


