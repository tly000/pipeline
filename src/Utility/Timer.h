#pragma once
#include <chrono>

/*
 * Timer.h
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

struct Timer{
	void start();
	uint64_t stop();
	std::chrono::steady_clock::time_point startPoint, stopPoint;
};


