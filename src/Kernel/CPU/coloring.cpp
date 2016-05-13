#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include <iostream>
#include <cmath>
#include "../../Type/Vec.h"

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
	CPUImage<float>& iterInput, CPUImage<Vec<3,float>>& colorOutput) {

	float iter = iterInput.at(globalID.x,globalID.y);
	if(iter == MAXITER){
		colorOutput.at(globalID.x,globalID.y) = { 1,1,1};
	}else{
		float val = fabs(fmod(iter / 100,2)-1);
		colorOutput.at(globalID.x,globalID.y) ={ 0, val * 0.7f , val };
	}
}
