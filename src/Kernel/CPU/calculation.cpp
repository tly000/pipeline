#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include "../Multisampling.h"
#include "../Type/Complex.h"

/*
 * calculation.cpp
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

extern "C" void mandelbrotKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<float>& iterOutput) {

	Complex c = image.at(globalID.x,globalID.y);
	Complex z{
		floatToType(0),
		floatToType(0)
	};

	unsigned i = 0;
	while(i < MAXITER && cfabs2(z) <= BAILOUT){
		z = FORMULA;
		i++;
	}

	iterOutput.at(globalID.x,globalID.y) = i;
}

