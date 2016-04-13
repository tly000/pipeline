#pragma once
#include <cstdint>

/*
 * Buffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T>
struct Buffer{
	Buffer(std::size_t elemCount)
		:elemCount(elemCount){}

	std::size_t getElemCount() const{
		return elemCount;
	}
protected:
	std::size_t elemCount;
};




