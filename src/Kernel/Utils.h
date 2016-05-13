#pragma once

/*
 * Utils.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

#include "Type/StandardTypes.h"

inline uint64_t random(uint64_t seed){
	return (seed * (uint64_t)(0x5DEECE66DL) + (uint64_t)(0xB)) & (((uint64_t)(1) << 48L) - 1);
}


