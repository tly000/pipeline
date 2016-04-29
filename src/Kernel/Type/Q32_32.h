#pragma once

/*
 * Q32_32.h
 *
 *  Created on: 21.04.2016
 *      Author: tly
 */

#include "../Type/StandardTypes.h"

#ifdef __OPENCL_VERSION__
	typedef uint2 Q32_32;
#else
	#include <math.h>

	typedef struct{
		uint32_t x,y;
	} Q32_32;
#endif

inline Q32_32 tadd(const Q32_32 a,const Q32_32 b){
	int64_t r = (int64_t)(a.y) + (int64_t)(b.y);
	return (Q32_32){
		(int32_t)(a.x + b.x + (r >> 32)),
		(int32_t)(r & 0xFFFFFFFF)
	};
}

inline Q32_32 tsub(const Q32_32 a,const Q32_32 b){
	int64_t r = ((int64_t)(a.x) << 32) - ((int64_t)(b.x) << 32);
	return (Q32_32){
		(int32_t)((r & 0xFFFFFFFF00000000) >> 32),
		(int32_t)(a.y - b.y + (r & 0xFFFFFFFF))
	};
}

inline Q32_32 tmul(const Q32_32 a,const Q32_32 b){
	uint64_t hihi = (uint64_t)(a.x) * (uint64_t)(b.x),
			 hilo = (uint64_t)(a.x) * (uint64_t)(b.y),
			 lohi = (uint64_t)(a.y) * (uint64_t)(b.x),
			 lolo = (uint64_t)(a.y) * (uint64_t)(b.y);
	return (Q32_32){
		hihi + (hilo >> 32) + (lohi >> 32),
		(hilo & 0xFFFFFFFF) + (lohi & 0xFFFFFFFF) + (lolo >> 32)
	};

}

inline Q32_32 floatToQ32_32(const float a){
	return	(Q32_32){
		(int32_t)(a),
		(int32_t)(fmod(a,1) * ((uint64_t)(2) << 32))
	};
}

inline Q32_32 tdiv(const Q32_32 a,const Q32_32 b){
	return floatToQ32_32(0);
}

inline float tofloat(const Q32_32 a){
	return	(float)(a.x) + (float)(a.y / ((uint64_t)(2) << 32));
}



