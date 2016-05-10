#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include "../Utils.h"
#include "../Multisampling.h"

/*
 * reduction.cpp
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

extern "C" void reductionKernel(const Range& globalID,const Range& localID,const CPUImage<float>& input,CPUImage<float>& output){
	if(MULTISAMPLING_ENABLED){
		output.at(globalID.x,globalID.y) = input.at(globalID.x,globalID.y);
	}else{
		output.at(globalID.x,globalID.y) = input.at(globalID.x,globalID.y);
	}
}


