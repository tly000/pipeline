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

	virtual T* getDataPointer() = 0;
	virtual const T* getDataPointer() const = 0;

	virtual ~Buffer() = default;
protected:
	std::size_t elemCount;
};




