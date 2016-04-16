#include "position.h"

/*
 * position.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

void positionKernel(const Range& globalID,const Range& localID,CPUImage<Complex>& image){
	int w = image.getWidth();
	int h = image.getHeight();
	image.at(globalID.x,globalID.y) = Complex{
		fromfloat(3 * (float(globalID.x) - w/2) / w),
		fromfloat(3 * (float(globalID.y) - h/2) / h)
	};
}



