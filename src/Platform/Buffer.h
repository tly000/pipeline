#pragma once
#include <cstdint>
#include <vector>

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

	virtual void copyToBuffer(std::vector<T>& buffer) const = 0;
	virtual void copyFromBuffer(const std::vector<T>& buffer,std::size_t offset, std::size_t n) = 0;

	virtual ~Buffer() = default;
protected:
	std::size_t elemCount;
};




