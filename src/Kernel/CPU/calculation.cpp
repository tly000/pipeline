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

	Range modifiedRange =
		MULTISAMPLING_ENABLED ?
		Range{
			globalID.x * MULTISAMPLING_SIZE + localID.x,
			globalID.y * MULTISAMPLING_SIZE + localID.y,
			0
		} :
		globalID;
	Complex c = image.at(modifiedRange.x,modifiedRange.y);
	Complex z{
		floatToType(0),
		floatToType(0)
	};

	unsigned i = 0;
	while(i < MAXITER && cfabs2(z) <= BAILOUT){
		z = cadd(csqr(z),c);
		i++;
	}

	iterOutput.at(modifiedRange.x,modifiedRange.y) = i;
}

