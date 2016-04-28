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
using namespace std;

#endif

typedef struct{
	uint32_t words[FixedSize];
} Fixed;

#define fixedGetSign(f) (f.words[0] >> 31)
#define fixedSetSign(f,s) (s ? (f.words[0] |= (1 << 31)) : (f.words[0] &= ~(1 << 31)))

inline bool fixedCompareLessThan(Fixed a,Fixed b){
	//remove sign to ensure correct calculation
	fixedSetSign(a,false);
	fixedSetSign(b,false);

	#pragma unroll
	for(int i =0; i < FixedSize; i++){
		if(a.words[i] > b.words[i]){
			return false;
		}else if(a.words[i] < b.words[i]){
			return true;
		}
	}
	return false;
}

inline Fixed fixedAdd(Fixed a,Fixed b){
	bool aSign = fixedGetSign(a);

	//remove sign to ensure correct calculation
	fixedSetSign(a,false);
	fixedSetSign(b,false);
	Fixed c;
	uint64_t carry = 0;
	uint32_t c1 = 0,c2 = 0;

	#pragma unroll
	for(int i =FixedSize-1; i >= 0; i--){
		uint32_t result = a.words[i] + b.words[i];
		c1 = result < a.words[i];
		c.words[i] = result + carry;
		c2 = c.words[i] < result;
		carry = c1 | c2;
	}
	fixedSetSign(c,aSign);
	return c;
}


inline Fixed fixedSub(Fixed a,Fixed b){
	bool aSign = fixedGetSign(a);

	//remove sign to ensure correct calculation
	fixedSetSign(a,false);
	fixedSetSign(b,false);
	Fixed c;
	uint64_t carry = 0;
	uint32_t c1 = 0,c2 = 0;

	#pragma unroll
	for(int i =FixedSize-1; i >= 0; i--){
		uint32_t result = a.words[i] - b.words[i];
		c1 = result > a.words[i];
		c.words[i] = result - carry;
		c2 = c.words[i] > result;
		carry = c1 | c2;
	}
	fixedSetSign(c,aSign);
	return c;
}

inline Fixed tadd(const Fixed a,const Fixed b){
	bool aSign = fixedGetSign(a),
		 bSign = fixedGetSign(b);

	if(aSign ^ bSign){
		if(fixedCompareLessThan(a,b)){
			return fixedSub(b,a);
		}else{
			return fixedSub(a,b);
		}
	}else{
		return fixedAdd(a,b);
	}
}

inline Fixed tsub(const Fixed a,Fixed b){
	bool aSign = fixedGetSign(a),
		 bSign = fixedGetSign(b);

	if(aSign ^ bSign){
		return fixedAdd(a,b);
	}else{
		if(fixedCompareLessThan(a,b)){
			fixedSetSign(b,!bSign);
			return fixedSub(b,a);
		}else{
			return fixedSub(a,b);
		}
	}
}

inline Fixed tmul(Fixed a,Fixed b){
	Fixed c;
	bool cSign = fixedGetSign(a) ^ fixedGetSign(b);

	//remove sign to ensure correct calculation
	fixedSetSign(a,false);
	fixedSetSign(b,false);
	uint64_t carry[FixedSize] = {};

	#pragma unroll
	for(int i = FixedSize-1; i >= 0; i--){
		#pragma unroll
		for(int j = FixedSize-1; j >= 0; j--){
			uint16_t k = i + j;
			uint64_t r = (uint64_t)(a.words[i]) * (uint64_t)(b.words[j]);
			if(k <= FixedSize){
				if(k < FixedSize) carry[k] += (uint32_t)(r & 0xFFFFFFFF);
				if(k > 0) carry[k-1] += (uint32_t)(r >> 32);
			}
		}
	}

    uint64_t currentCarry = 0;

	#pragma unroll
	for (int i = FixedSize - 1; i >= 0; i--) {
		currentCarry += carry[i];
		c.words[i] = (uint32_t)(currentCarry & 0xFFFFFFFF);
		currentCarry >>= 32;
	}
	fixedSetSign(c,cSign);
	return c;
}

inline Fixed fromfloat(float a){
	bool negate = a < 0;
	Fixed f;
	a = fabs(a);
	f.words[0] = (uint32_t)(a);
	f.words[1] = (uint32_t)(fmod(a,1) * ((uint64_t)(1) << 32));

	#pragma unroll
	for(int i = 2; i <FixedSize; i++){
		f.words[i] = 0;
	}
	fixedSetSign(f,negate);

	return f;
}

inline Fixed tdiv(const Fixed a,const Fixed b){
	return (Fixed){};
}

inline float tofloat(Fixed a){
	bool negate = fixedGetSign(a);

	fixedSetSign(a,false);
	return (
		(float)(a.words[0]) +
		(float)((double)(a.words[1]) / ((uint64_t)(1) << 32))
	) * (negate ? -1 : 1);
}

inline Fixed tmulKaratsuba(Fixed a,Fixed b){
	Fixed c;
	bool cSign = fixedGetSign(a) ^ fixedGetSign(b);

	//remove sign to ensure correct calculation
	fixedSetSign(a,false);
	fixedSetSign(b,false);
	uint64_t carry[FixedSize];

	for(int i = 0; i <FixedSize; i++){
		carry[i] = 0;
	}

	for(int i = FixedSize-1; i >= 0; i--){
		for(int j = FixedSize-1; j >= 0; j--){
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
	fixedSetSign(c,cSign);
	return c;
}

//inline Fixed tneg(const Fixed a){
//	Fixed c;
//	uint64_t carry = 1;
//	for(int i =FixedSize-1; i >= 0; i--){
//		uint32_t result = ~a.words[i];
//		c.words[i] = result + carry;
//		carry = c.words[i] < result;
//	}
//	return c;
//}
//
//inline Fixed tadd(const Fixed a,const Fixed b){
//	Fixed c;
//	uint64_t carry = 0;
//	uint32_t c1 = 0,c2 = 0;
//	for(int i =FixedSize-1; i >= 0; i--){
//		uint32_t result = a.words[i] + b.words[i];
//		c1 = result < a.words[i];
//		c.words[i] = result + carry;
//		c2 = c.words[i] < result;
//		carry = c1 | c2;
//	}
//	return c;
//}
//
//
//inline Fixed tsub(const Fixed a,const Fixed b){
//	Fixed c;
//	uint64_t carry = 0;
//	uint32_t c1 = 0,c2 = 0;
//	for(int i =FixedSize-1; i >= 0; i--){
//		uint32_t result = a.words[i] - b.words[i];
//		c1 = result > a.words[i];
//		c.words[i] = result - carry;
//		c2 = c.words[i] > result;
//		carry = c1 | c2;
//	}
//	return c;
//}
//
//inline Fixed tmul(Fixed a,Fixed b){
//	Fixed c;
//	//c.sign = a.sign * b.sign;
//	uint64_t carry[FixedSize] = {};
//
//	bool aNegative = (int32_t)(a.words[0]) < 0;
//	if(aNegative){
//		a = tneg(a);
//	}
//	bool bNegative = (int32_t)(b.words[0]) < 0;
//	if(bNegative){
//		b = tneg(b);
//	}
//	bool cNegative = aNegative ^ bNegative;
//
//	for(int i = FixedSize-1; i >= 0; i--){
//		for(int j = FixedSize-1; j >= 0; j--){
//			uint16_t k = i + j;
//			uint64_t r = (uint64_t)(a.words[i]) * (uint64_t)(b.words[j]);
//			if(k <= FixedSize){
//				if(k < FixedSize) carry[k] += (uint32_t)(r & 0xFFFFFFFF);
//				if(k > 0) carry[k-1] += (uint32_t)(r >> 32);
//			}
//		}
//	}
//
//    uint64_t currentCarry = 0;
//	for (int i = FixedSize - 1; i >= 0; i--) {
//		currentCarry += carry[i];
//		c.words[i] = (uint32_t)(currentCarry & 0xFFFFFFFF);
//		currentCarry >>= 32;
//	}
//	if(cNegative){
//		c = tneg(c);
//	}
//
//	return c;
//}
//
//inline Fixed fromfloat(float a){
//	bool negate = a < 0;
//	Fixed f;
//	a = fabs(a);
//	f.words[0] = (uint32_t)(a);
//	f.words[1] = (uint32_t)(fmod(a,1) * ((uint64_t)(1) << 32));
//	for(int i = 2; i <FixedSize; i++){
//		f.words[i] = 0;
//	}
//
//	return negate ? tneg(f) : f;
//}
//
//inline Fixed tdiv(const Fixed a,const Fixed b){
//	return (Fixed){};
//}
//
//inline float tofloat(Fixed a){
//	bool negate = a.words[0] >= 0x10000000;
//	if(negate){
//		a = tneg(a);
//	}
//	return (
//		(float)(a.words[0]) +
//		(float)((double)(a.words[1]) / ((uint64_t)(1) << 32))
//	) * (negate ? -1 : 1);
//}


