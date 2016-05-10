#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include <iostream>
#include <cmath>

/*
 * coloring.cpp
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

#ifndef MAXITER
#define MAXITER 64
#endif

#ifndef BAILOUT
#define BAILOUT 4
#endif


extern "C" void coloringKernel(
	const Range& globalID, const Range& localID,
	CPUImage<float>& iterInput, CPUImage<uint32_t>& colorOutput) {

	uint32_t iter = iterInput.at(globalID.x,globalID.y);
	struct{
		uint8_t r,g,b,a;
	} color;
	if(iter == MAXITER){
		color = { 255, 255, 255, 255};
	}else{
		float val = fabs(fmod((float)iter / 100,2)-1);
		color = { 0, uint8_t(val * 150), uint8_t(val * 255), 255};
	}
	colorOutput.at(globalID.x,globalID.y) = reinterpret_cast<uint32_t&>(color);
}
