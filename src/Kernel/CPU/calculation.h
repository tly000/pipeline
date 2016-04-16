#pragma once

/*
 * calculation.h
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

#include "../../Platform/CPU/CPUImage.h"
#include "../Type/Complex.h"
#include "../../Type/Range.h"

void mandelbrotKernel(const Range& globalID,const Range& localID,CPUImage<Complex>& image,CPUImage<unsigned>& iterOutput);


