#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include "../Utils.h"
#include "../Multisampling.h"
#include "../Type/Complex.h"
#include <iostream>

/*
 * position.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

extern "C" void positionKernel(const Range& globalID,const Range& localID,CPUImage<Complex>& image,Type& offsetReal,Type& offsetImag,Complex& scaleFactor){
	int w = image.getWidth();
	int h = image.getHeight();
	Complex offset ={
		offsetReal, offsetImag
	};
	float fx = globalID.x, fy = globalID.y;
	if(MULTISAMPLING_ENABLED){
		uint64_t seed = globalID.x + globalID.y + localID.x + localID.y;

		if(MULTISAMPLING_PATTERN == JITTERING){
			float xR = uint32_t(seed = random(seed)) / (float)UINT_MAX;
			float yR = uint32_t(seed = random(seed)) / (float)UINT_MAX;
			fx += xR - 0.5;
			fy += yR - 0.5;
		}
	}
	Complex pos = {
		floatToType(2 * (fx - w/2) / w),
		floatToType(2 * (fy - h/2) / h * (float)h/w)
	};

	image.at(globalID.x,globalID.y) = cadd(cmul(scaleFactor,pos),offset);
}


