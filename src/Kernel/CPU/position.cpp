#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include "../Utils.h"
#include "../Multisampling.h"
#include "../Type/Complex.h"

/*
 * position.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

extern "C" void positionKernel(const Range& globalID,const Range& localID,CPUImage<Complex>& image,Type& offsetReal,Type& offsetImag,Type& scale){
	int w = image.getWidth();
	int h = image.getHeight();
	Complex pos = {
		floatToType(2 * (float(globalID.x) - w/2) / w),
		floatToType(2 * (float(globalID.y) - h/2) / h * (float)h/w)
	};
	Complex scaleFactor ={
		scale, floatToType(0)
	};
	Complex offset ={
		offsetReal, offsetImag
	};
	if(MULTISAMPLING_ENABLED){
		struct{
			float x,y;
		} floatPos = {
			globalID.x,
			globalID.y
		};
		uint64_t seed = globalID.x + globalID.y + localID.x + localID.y;

		switch(MULTISAMPLING_PATTERN){
		case MULTISAMPLING_JITTERGRID:{
			float xR = uint32_t(seed = random(seed)) / (float)UINT_MAX;
			float yR = uint32_t(seed = random(seed)) / (float)UINT_MAX;
			floatPos.x += (localID.x + xR - 0.5) / (float)(MULTISAMPLING_SIZE * w);
			floatPos.y += (localID.y + yR - 0.5) / (float)(MULTISAMPLING_SIZE * h);
			break;
		}
		//case MULTISAMPLING_GAUSSIAN:
		case MULTISAMPLING_UNIFORMGRID:
		default:
			floatPos.x += localID.x / (float)(MULTISAMPLING_SIZE * w);
			floatPos.y += localID.y / (float)(MULTISAMPLING_SIZE * h);
			break;
		}

		Complex pos = {
			floatToType(2 * ((float)(floatPos.x) - w/2) / w),
			floatToType(2 * ((float)(floatPos.y) - h/2) / h * (float)h/w)
		};

		image.at(globalID.x * MULTISAMPLING_SIZE + localID.x,
		         globalID.y * MULTISAMPLING_SIZE + localID.y) = cadd(cmul(scaleFactor,pos),offset);
	}else{
		Complex pos = {
			floatToType(2 * ((float)(globalID.x) - w/2) / w),
			floatToType(2 * ((float)(globalID.y) - h/2) / h * (float)h/w)
		};
		image.at(globalID.x,globalID.y) = cadd(cmul(scaleFactor,pos),offset);
	}
}


