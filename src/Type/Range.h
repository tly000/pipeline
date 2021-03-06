#pragma once
#include <cstdint>

/*
 * Range.h
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

struct Range{
	std::uint32_t x = 1,y = 1,z = 1;
};

constexpr bool operator==(const Range& a,const Range& b){
	return a.x == b.x && a.y == b.y && a.z == b.z;
}

