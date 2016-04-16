#include "calculation.h"
#include <iostream>

/*
 * calculation.cpp
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

const uint32_t MAXITER = 64;
const float BAILOUT = 4;

void mandelbrotKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<unsigned>& iterOutput) {

	Complex c = image.at(globalID.x,globalID.y);
	Complex z{0,0};

	unsigned i = 0;
	while(i < MAXITER && tofloat(cabs2(z)) <= BAILOUT){
		z = cadd(cmul(z,z),c);
		i++;
	}

	iterOutput.at(globalID.x,globalID.y) = i;
}

