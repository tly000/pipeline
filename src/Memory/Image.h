#pragma once
/*
 * Image.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct Image{
	Image(size_t width,size_t height)
		:width(width),
		 height(height){}

	size_t getWidth() const{
		return width;
	}

	size_t getHeight() const{
		return height;
	}
protected:
	size_t width, height;
};




