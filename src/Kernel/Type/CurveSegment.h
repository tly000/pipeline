#pragma once
#include "StandardTypes.h"

/*
 * CurveSegment.h
 *
 *  Created on: 15.06.2016
 *      Author: tly
 */

typedef struct{
	float2 start,end;
	float paramA,paramB;
	int repetition,step;
} CurveSegment;

#ifdef __OPENCL_VERSION__
#define GLOBAL_ATTRIB global
#else
#define GLOBAL_ATTRIB
#endif

inline float curveSample(const GLOBAL_ATTRIB CurveSegment* curve, uint32_t curveSize, float t){
	t =clamp(t,curve[0].start.x,curve[curveSize-1].end.x);

	uint32_t i = 0;
	for(; i < curveSize; i++){
		if(t >= curve[i].start.x && t <= curve[i].end.x){
			break;
		}
	}

	const GLOBAL_ATTRIB CurveSegment* seg = curve+i;
	float t1 = (t - seg->start.x)/(seg->end.x - seg->start.x);

	if(seg->repetition > 0){
		t1 = fmod(seg->repetition * t1,1);
	}else if(seg->repetition < 0){
		t1 *= abs(seg->repetition);
		t1 = fabs(fmod(t1+1,2)-1);
	}
	if(seg->step != 0){
		t1 = round(seg->step * t1) / seg->step;
	}

	t1 = 1 - pow(1-pow(t1,seg->paramA),seg->paramB);
	return mix(seg->start.y,seg->end.y,t1);
}


