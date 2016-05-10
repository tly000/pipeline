#pragma once

/*
 * Utils.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

#include "Type/StandardTypes.h"

uint64_t random(uint64_t seed){
	return (seed * 0x5DEECE66DL + 0xBL) & ((1ULL << 48) - 1);
}


