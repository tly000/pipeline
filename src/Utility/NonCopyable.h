#pragma once
/*
 * NonCopyable.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

struct NonCopyable{
	NonCopyable() = default;

	NonCopyable& operator=(const NonCopyable&) = delete;
	NonCopyable& operator=(NonCopyable&&) = delete;

	NonCopyable(const NonCopyable&) = delete;
	NonCopyable(NonCopyable&&) = default;
};



