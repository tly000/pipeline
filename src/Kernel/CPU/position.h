#pragma once
/*
 * position.h
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

#include "../../Platform/CPU/CPUImage.h"
#include "../Type/Complex.h"
#include "../../Type/Range.h"

void positionKernel(const Range& globalID,const Range& localID,CPUImage<Complex>& image);




