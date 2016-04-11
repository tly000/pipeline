#pragma once
/*
 * Buffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T>
struct Buffer{
	Buffer(size_t elemCount)
		:elemCount(elemCount){}

	size_t getElemCount() const{
		return elemCount;
	}
protected:
	size_t elemCount;
};




