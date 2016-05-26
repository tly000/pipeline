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

inline Complex f(const Complex z,const Complex c){
	return FORMULA;
}

extern "C" void mandelbrotKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<float>& iterOutput,const Type& cReal, const Type& cImag) {

	const Complex juliaC = {
		cReal, cImag
	};
	const Complex c = JULIAMODE ? juliaC : image.at(globalID.x,globalID.y);
	Complex z = JULIAMODE ? image.at(globalID.x,globalID.y) : (Complex){
		floatToType(0),
		floatToType(0)
	};
	Complex fastZ = z;

	unsigned i = 0;

	float l = 0;

	while(i < MAXITER && (l = tofloat(cabs2(z))) <= BAILOUT){
		z = f(z,c);
		if(CYCLE_DETECTION){
			fastZ = f(f(fastZ,c),c);
			if(tofloat(cabs2(csub(fastZ,z))) < 1e-10){
				if(!CYCLE_DETECTION_VISUALIZE){
					i = MAXITER;
				}
				break;
			}
		}
		i++;
	}

	iterOutput.at(globalID.x,globalID.y) = i ;
}

