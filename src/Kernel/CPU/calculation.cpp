#include "calculation.h"
#include <iostream>

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
#define BAILOUT 64
#endif

void mandelbrotKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<unsigned>& iterOutput) {

	Complex c = image.at(globalID.x,globalID.y);
	Complex z{
		floatToType(0),
		floatToType(0)
	};

	unsigned i = 0;
	while(i < MAXITER && cfabs2(z) <= BAILOUT){
		z = cadd(csqr(z),c);
		i++;
	}

	iterOutput.at(globalID.x,globalID.y) = i;
}

