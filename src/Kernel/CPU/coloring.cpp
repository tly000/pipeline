#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include <iostream>
#include <cmath>
#include "../../Type/Vec.h"
#include "../Type/Complex.h"

const static float PI = 3.1415926;

/*
 * coloring.cpp
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

float3 gradientSample(const CPUBuffer<float3>& gradient, float t){
	t *= gradient.getElemCount() - 1;
	float fT = fmod(t,1);
	int iT = floor(t);

	if(iT >= gradient.getElemCount() - 1){
		return gradient.at(gradient.getElemCount() - 1);
	}else{
		return (1-fT) * gradient.at(iT) + fT * gradient.at(iT+1);
	}
}

extern "C" void iterationGradient(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB) {
	float iter = iterInput.at(globalID.x,globalID.y);
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,iter / float(MAXITER));
}

extern "C" void angleGradient(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB) {
	Complex p = processedPositionImage.at(globalID.x,globalID.y);
	float angle = atan2(tofloat(p.imag),tofloat(p.real));
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,fmod((angle+PI)/(2*PI),1));
}

extern "C" void flatColor(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB) {
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,0);
}

extern "C" void lengthGradient(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB) {

	Complex p = processedPositionImage.at(globalID.x,globalID.y);
	float length = sqrtf(tofloat(cabs2(p)));
	length = std::min(length,1.0f);
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,length > 1 ? 1/length : length);
}

extern "C" void angleMixGradient(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB) {
	Complex p = processedPositionImage.at(globalID.x,globalID.y);

	float angle = atan2(tofloat(p.imag),tofloat(p.real));
	float length = sqrtf(tofloat(cabs2(p))) / sqrtf(BAILOUT);
	length = length > 1 ? 1/length : length;
	colorOutput.at(globalID.x,globalID.y) =
		(1-length) * gradientSample(gradientA,fmod((angle+PI)/(2*PI),1)) +
		length * gradientSample(gradientB,gradientB.getElemCount()-1);
}

extern "C" void coloringKernel(
	const Range& globalID, const Range& localID,
	CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	CPUImage<Vec<3,float>>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB) {

	float& iter = iterInput.at(globalID.x,globalID.y);
	const Complex& p = processedPositionImage.at(globalID.x,globalID.y);

	if(SMOOTH_MODE && iter != MAXITER){
		iter -= log(log(tofloat(cabs2(p)))/log((float)MAXITER)) / log((float)SMOOTH_EXP);
	}
	if(iter == MAXITER){
		INSIDE_COLORING_KERNEL(globalID,localID,iterInput,processedPositionImage,colorOutput,gradientB,gradientA);
	}else{
		OUTSIDE_COLORING_KERNEL(globalID,localID,iterInput,processedPositionImage,colorOutput,gradientA,gradientB);
	}
}



