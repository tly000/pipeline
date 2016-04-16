#include "Timer.h"

/*
 * Timer.cpp
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

void Timer::start() {
	startPoint = std::chrono::steady_clock::now();
}

uint64_t Timer::stop() {
	stopPoint = std::chrono::steady_clock::now();
	return std::chrono::duration_cast<std::chrono::microseconds>(stopPoint - startPoint).count();
}



