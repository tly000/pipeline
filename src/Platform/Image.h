#pragma once
#include <cstddef>

/*
 * Image.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct Image{
	Image(std::size_t width,std::size_t height)
		:width(width),
		 height(height){}

	std::size_t getWidth() const{
		return width;
	}

	std::size_t getHeight() const{
		return height;
	}

	virtual ~Image() = default;
protected:
	std::size_t width, height;
};




