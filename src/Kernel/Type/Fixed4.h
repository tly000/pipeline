#pragma once

/*
 * Fixed4.h
 *
 *  Created on: 22.04.2016
 *      Author: tly
 */

#if defined(__OPENCL_VERSION__) && SEPARATE_IMPL

typedef union{ uint4 v; uint words[4];} Fixed4;
#define FixedSize 4

typedef Fixed4 Fixed;

#define fixedGetSign(f) (f.v.x >> 31)
#define fixedSetSign(f,s) (s ? (f.v.x |= (1 << 31)) : (f.v.x &= ~(1 << 31)))

inline bool fixedCompareLessThan(Fixed4 a,Fixed4 b){
	//remove sign to ensure correct calculation
	fixedSetSign(a,false);
	fixedSetSign(b,false);

	int4 cmp = (a.v > b.v) - (a.v < b.v);
	return (cmp.x * 8 + cmp.y * 4 + cmp.z * 2 + cmp.w) > 0;
}

inline Fixed4 fixedAdd(Fixed4 a,Fixed4 b){
	bool aSign = fixedGetSign(a);
	//remove sign to ensure correct calculation
	fixedSetSign(a,false);
	fixedSetSign(b,false);

	uint4 result = a.v + b.v;
	uint4 carry = convert_uint4(result < a.v) & (uint4)(0,1,1,1);
	Fixed4 c; c.v = result + carry.yzwx;

	fixedSetSign(c,aSign);
	return c;
}

inline Fixed4 fixedSub(Fixed4 a,Fixed4 b){
	bool aSign = fixedGetSign(a);
	//remove sign to ensure correct calculation
	fixedSetSign(a,false);
	fixedSetSign(b,false);

	uint4 result = a.v - b.v;
	uint4 carry = convert_uint4(result > a.v) & (uint4)(0,1,1,1);
	Fixed4 c; c.v = result - carry.yzwx;

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

inline Fixed4 tmul(Fixed4 a,Fixed4 b){
	Fixed c;
	bool cSign = fixedGetSign(a) ^ fixedGetSign(b);

	//remove sign to ensure correct calculation
	fixedSetSign(a,false);
	fixedSetSign(b,false);
	union {
		ulong4 v;
		ulong words[4];
	} carry;

	ulong4 aLong = convert_ulong4(a.v);
	ulong4 bLong = convert_ulong4(b.v);

	ulong4 mult;
	mult = aLong.x * bLong;
		carry.v.s0123 = (mult.s0123 & 0xFFFFFFFF);
		carry.v.s012 += (mult.s123 >> 32);
	mult = aLong.y * bLong;
		carry.v.s123 += (mult.s012 & 0xFFFFFFFF);
		carry.v.s123 += (mult.s123 >> 32);
	mult.s012 = aLong.z * bLong.s012;
		carry.v.s23 += (mult.s01 & 0xFFFFFFFF);
		carry.v.s23 += (mult.s12 >> 32);
	mult.s01 = aLong.w * bLong.s01;
		carry.v.s3 += (mult.s0 & 0xFFFFFFFF);
		carry.v.s3 += (mult.s1 >> 32);

	ulong currentCarry = 0;
	for (int i = FixedSize - 1; i >= 0; i--) {
		currentCarry += carry.words[i];
		c.words[i] = (uint32_t)(currentCarry & 0xFFFFFFFF);
		currentCarry >>= 32;
	}
	fixedSetSign(c,cSign);
	return c;
}

inline Fixed4 fromfloat(float a){
	bool negate = a < 0;
	Fixed4 f;
	a = fabs(a);
	f.v.x = (uint32_t)(a);
	f.v.y = (uint32_t)(fmod(a,1) * ((uint64_t)(1) << 32));
	fixedSetSign(f,negate);

	return f;
}

inline Fixed4 tdiv(const Fixed4 a,const Fixed4 b){
	return (Fixed4){};
}

inline float tofloat(Fixed4 a){
	bool negate = fixedGetSign(a);

	fixedSetSign(a,false);
	return (
		(float)(a.v.x) +
		(float)((double)(a.v.y) / ((uint64_t)(1) << 32))
	) * (negate ? -1 : 1);
}

#else

#define FixedSize 4
#include "FixedN.h"

#endif
