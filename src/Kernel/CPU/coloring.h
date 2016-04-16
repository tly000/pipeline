#pragma once

/*
 * coloring.h
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

#include "../../Platform/CPU/CPUImage.h"
#include "../../Type/Range.h"

extern "C" void coloringKernel(const Range& globalID,const Range& localID,CPUImage<uint32_t>& iterInput,CPUImage<uint32_t>& colorOutput);







