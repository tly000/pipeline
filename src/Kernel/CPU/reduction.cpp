#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include "../Utils.h"
#include "../Multisampling.h"
#include "../../Type/Vec.h"
#include <iostream>

/*
 * reduction.cpp
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

extern "C" void reductionKernel(const Range& globalID,const Range& localID,const CPUImage<Vec<3,float>>& input,CPUImage<unsigned>& output){
	Vec<3,float> val{ 0,0,0 };
	if(MULTISAMPLING_ENABLED){
		for(int i = 0; i < MULTISAMPLING_SIZE; i++){
			for(int j = 0; j < MULTISAMPLING_SIZE; j++){
				val += input.at(globalID.x * MULTISAMPLING_SIZE + i,globalID.y * MULTISAMPLING_SIZE + j);
			}
		}
		val /= (MULTISAMPLING_SIZE * MULTISAMPLING_SIZE);
	}else{
		val = input.at(globalID.x,globalID.y);
	}
	Vec<4,uint8_t> rgba(255 * val);
	rgba[3] = 255; //making the color opaque
	output.at(globalID.x,globalID.y) = reinterpret_cast<uint32_t&>(rgba);
}


