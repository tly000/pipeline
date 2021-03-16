#pragma once
/*
 * StandardTypes.h
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

#ifdef __OPENCL_VERSION__

typedef short int16_t;
typedef char int8_t;
typedef int int32_t;
typedef long int64_t;
typedef unsigned char uint8_t;
typedef unsigned short uint16_t;
typedef unsigned int uint32_t;
typedef unsigned long uint64_t;

#elif defined(__cplusplus)
#include <stdint.h>
#include <math.h>
#include <stdlib.h>
#include <limits.h>
#include <algorithm>
#include "../../src/Type/Vec.h"

using std::clamp;

using float3 = Vec<3,float>;
using float4 = Vec<4,float>;
using int2 = Vec<2,int32_t>;

struct float2 {
	float x, y;
};

template<typename T> T mix(T a,T b,T t){
	return (1-t) * a + t * b;
}

#else
#error "system not supported"
#endif



