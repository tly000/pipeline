#include "coloring.h"
#include <iostream>
#include <cmath>

/*
 * coloring.cpp
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */


void coloringKernel(
	const Range& globalID, const Range& localID,
	CPUImage<uint32_t>& iterInput, CPUImage<uint32_t>& colorOutput) {

	float val = std::sqrt(iterInput.at(globalID.x,globalID.y) / 64.0f);
	struct{
		uint8_t r,g,b,a;
	} color{ 0, uint8_t(val * 255), uint8_t((1-val) * 255), 255};
	colorOutput.at(globalID.x,globalID.y) = reinterpret_cast<uint32_t&>(color);
}
