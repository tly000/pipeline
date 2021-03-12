//Caution: no #pragma once!

/*
 * fixedPointN.h
 *
 *  Created on: 22.04.2016
 *      Author: tly
 */

#include "../MacroTools.h"
#include "StandardTypes.h"
#ifndef FixedSize
#define FixedSize 8
#endif

#define Fixed _concat(Fixed,FixedSize)
#define floatToFixed _concat(floatTo,Fixed)

#ifndef __OPENCL_VERSION__
	#include <math.h>
	using namespace std;
	#define UNROLL
#else
	#define UNROLL #pragma unroll
#endif

#define DIV_PRECISION 4
#define USE_TWOS_COMPLEMENT 0

#if !USE_TWOS_COMPLEMENT

	#define SAVE_SIGN_IN_MSB 0

	#if SAVE_SIGN_IN_MSB
		typedef struct{
			uint32_t words[FixedSize];
		} Fixed;

		#define fixedGetSign(f) (f.words[0] >> 31)
		#define fixedSetSign(f,s) (s ? (f.words[0] |= (1 << 31)) : (f.words[0] &= ~(1 << 31)))
		#define fixedToggleSign(f) (f.words[0] ^= (1 << 31))
	#else
		typedef struct{
			uint32_t words[FixedSize];
			uint32_t sign;
		} Fixed;

		#define fixedGetSign(f) (f.sign)
		#define fixedSetSign(f,s) (f.sign = s)
		#define fixedToggleSign(f) (f.sign ^= 1)
	#endif
	#undef SAVE_SIGN_IN_MSB

	inline bool fixedCompareLessThan(Fixed a,Fixed b){
		//remove sign to ensure correct calculation
		fixedSetSign(a,false);
		fixedSetSign(b,false);

		UNROLL
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

		UNROLL
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

		UNROLL
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

	inline Fixed tneg(Fixed a){
		fixedSetSign(a,!fixedGetSign(a));
		return a;
	}

	inline Fixed tmul(Fixed a,Fixed b){
		Fixed c;
		bool cSign = fixedGetSign(a) ^ fixedGetSign(b);

		//remove sign to ensure correct calculation
		fixedSetSign(a,false);
		fixedSetSign(b,false);
		uint64_t carry[FixedSize] = {0};

		UNROLL
		for(int i = FixedSize-1; i >= 0; i--){
			UNROLL
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

		UNROLL
		for (int i = FixedSize - 1; i >= 0; i--) {
			currentCarry += carry[i];
			c.words[i] = (uint32_t)(currentCarry & 0xFFFFFFFF);
			currentCarry >>= 32;
		}
		fixedSetSign(c,cSign);
		return c;
	}

	inline Fixed tabs(Fixed a){
		fixedSetSign(a,false);
		return a;
	}

	inline Fixed floatToFixed(float a){
		bool negate = a < 0;
		a = fabs(a);

		Fixed f = {{
			(uint32_t)(a),
			(uint32_t)(fmod(a,1) * ((uint64_t)(1) << 32))
		}};
		fixedSetSign(f,negate);

		return f;
	}

	inline Fixed tsqrt(Fixed a){
		return a;
	}

#if defined(__OPENCL_VERSION___) && defined(cl_khr_fp64)
	#pragma OPENCL EXTENSION cl_khr_fp64 : enable
	#define conversionType double
#else
	#define conversionType float
#endif

	inline float tofloat(Fixed a){
		bool negate = fixedGetSign(a);

		fixedSetSign(a,false);
		return (
			(float)(a.words[0]) +
			(float)((conversionType)(a.words[1]) / ((uint64_t)(1) << 32))
		) * (negate ? -1 : 1);
	}

#undef conversionType

	inline Fixed tdiv(Fixed a,Fixed b){
		Fixed c;
		bool cSign = fixedGetSign(a) ^ fixedGetSign(b);

		//remove sign to ensure correct calculation
		fixedSetSign(a,false);
		fixedSetSign(b,false);
		Fixed x = floatToFixed(1/tofloat(b));

		UNROLL
		for(int i = 0; i < DIV_PRECISION; i++){
			x = tmul(x,tsub(floatToFixed(2),tmul(b,x)));
		}

		c = tmul(a,x);
		fixedSetSign(c,cSign);

		return c;
	}

#else

	typedef struct{
		uint32_t words[FixedSize];
	} Fixed;

	inline Fixed fixedNegate(const Fixed a){
		Fixed c;
		uint64_t carry = 1;
		UNROLL
		for(int i =FixedSize-1; i >= 0; i--){
			uint32_t result = ~a.words[i];
			c.words[i] = result + carry;
			carry = c.words[i] < result;
		}
		return c;
	}

	inline Fixed tadd(const Fixed a,const Fixed b){
		Fixed c;
		uint64_t carry = 0;
		uint32_t c1 = 0,c2 = 0;
		UNROLL
		for(int i =FixedSize-1; i >= 0; i--){
			uint32_t result = a.words[i] + b.words[i];
			c1 = result < a.words[i];
			c.words[i] = result + carry;
			c2 = c.words[i] < result;
			carry = c1 | c2;
		}
		return c;
	}


	inline Fixed tsub(const Fixed a,const Fixed b){
		Fixed c;
		uint64_t carry = 0;
		uint32_t c1 = 0,c2 = 0;
		UNROLL
		for(int i =FixedSize-1; i >= 0; i--){
			uint32_t result = a.words[i] - b.words[i];
			c1 = result > a.words[i];
			c.words[i] = result - carry;
			c2 = c.words[i] > result;
			carry = c1 | c2;
		}
		return c;
	}

	inline Fixed tmul(Fixed a,Fixed b){
		Fixed c;
		//c.sign = a.sign * b.sign;
		uint64_t carry[FixedSize] = {};

		bool aNegative = (int32_t)(a.words[0]) < 0;
		if(aNegative){
			a = fixedNegate(a);
		}
		bool bNegative = (int32_t)(b.words[0]) < 0;
		if(bNegative){
			b = fixedNegate(b);
		}
		bool cNegative = aNegative ^ bNegative;

		UNROLL
		for(int i = FixedSize-1; i >= 0; i--){
			UNROLL
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
		UNROLL
		for (int i = FixedSize - 1; i >= 0; i--) {
			currentCarry += carry[i];
			c.words[i] = (uint32_t)(currentCarry & 0xFFFFFFFF);
			currentCarry >>= 32;
		}
		if(cNegative){
			c = fixedNegate(c);
		}

		return c;
	}

	inline Fixed floatToFixed(float a){
		bool negate = a < 0;
		a = fabs(a);

		Fixed f{{
			[0] = (uint32_t)(a),
			[1] = (uint32_t)(fmod(a,1) * ((uint64_t)(1) << 32))
		}};

		return negate ? fixedNegate(f) : f;
	}

	inline float tofloat(Fixed a){
		bool negate = a.words[0] >= 0x10000000;
		if(negate){
			a = fixedNegate(a);
		}
		return (negate ? -1 : 1) * (
			(float)(a.words[0]) +
			(float)((double)(a.words[1]) / ((uint64_t)(1) << 32))
		);
	}

	inline Fixed tdiv(Fixed a,Fixed b){
		Fixed c;

		bool aNegative = (int32_t)(a.words[0]) < 0;
		if(aNegative){
			a = fixedNegate(a);
		}
		bool bNegative = (int32_t)(b.words[0]) < 0;
		if(bNegative){
			b = fixedNegate(b);
		}
		bool cNegative = aNegative ^ bNegative;
		Fixed x = floatToFixed(1/tofloat(b));

		UNROLL
		for(int i = 0; i < DIV_PRECISION; i++){
			x = tmul(x,tsub(floatToFixed(2),tmul(b,x)));
			//x = tadd(x,tmul(x,tsub(one,tmul(b,x))));
		}

		c = tmul(a,x);
		if(cNegative){
			c = fixedNegate(c);
		}

		return c;
	}
#endif

#undef FixedSize
#undef Fixed
#undef floatToFixed
#undef UNROLL
#undef USE_TWOS_COMPLEMENT
#undef DIV_PRECISION
