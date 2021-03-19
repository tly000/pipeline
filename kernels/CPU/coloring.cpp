#include "../../src/Type/Range.h"
#include "../Type/CPUImage.h"
#include "../Type/CPUBuffer.h"
#include <iostream>
#include <cmath>
#include "../../src/Type/Vec.h"
#include "../Type/Complex.h"
#include "../Type/CurveSegment.h"

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
	const CPUImage<float4>& statsImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB,
	const CPUBuffer<CurveSegment>& curveA,
	const CPUBuffer<CurveSegment>& curveB) {
	float iter = iterInput.at(globalID.x,globalID.y);
	float t = curveSample(curveA.getDataPointer(),curveA.getElemCount(),iter / float(MAXITER));
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,t);
}

extern "C" void expSmoothingGradient(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	const CPUImage<float4>& statsImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB,
	const CPUBuffer<CurveSegment>& curveA,
	const CPUBuffer<CurveSegment>& curveB) {
	float iter = statsImage.at(globalID.x,globalID.y)[0];
	float t = curveSample(curveA.getDataPointer(),curveA.getElemCount(),iter / float(MAXITER));
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,t);
}

extern "C" void trap1Gradient(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	const CPUImage<float4>& statsImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB,
	const CPUBuffer<CurveSegment>& curveA,
	const CPUBuffer<CurveSegment>& curveB) {
	float val = statsImage.at(globalID.x,globalID.y)[0];
	float iter = iterInput.at(globalID.x,globalID.y) / float(MAXITER);
	float t = curveSample(curveA.getDataPointer(),curveA.getElemCount(),val);
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,t);
}

extern "C" void angleGradient(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	const CPUImage<float4>& statsImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB,
	const CPUBuffer<CurveSegment>& curveA,
	const CPUBuffer<CurveSegment>& curveB) {
	Complex p = processedPositionImage.at(globalID.x,globalID.y);
	float angle = atan2(tofloat(p.imag),tofloat(p.real));
	float t = curveSample(curveA.getDataPointer(),curveA.getElemCount(),(angle+PI)/(2*PI));
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,t);
}

extern "C" void flatColor(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	const CPUImage<float4>& statsImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB,
	const CPUBuffer<CurveSegment>& curveA,
	const CPUBuffer<CurveSegment>& curveB) {
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,0);
}

extern "C" void lengthGradient(
	const Range& globalID, const Range& localID,
	const CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	const CPUImage<float4>& statsImage,
	CPUImage<float3>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB,
	const CPUBuffer<CurveSegment>& curveA,
	const CPUBuffer<CurveSegment>& curveB) {

	Complex p = processedPositionImage.at(globalID.x,globalID.y);
	float length = sqrtf(tofloat(cabs2(p))) / sqrtf(BAILOUT);
	float t = curveSample(curveA.getDataPointer(),curveA.getElemCount(),length > 1 ? 1/length : length);
	colorOutput.at(globalID.x,globalID.y) = gradientSample(gradientA,t);
}

extern "C" void coloringKernel(
	const Range& globalID, const Range& localID,
	CPUImage<float>& iterInput,
	const CPUImage<Complex>& processedPositionImage,
	const CPUImage<float4>& statsImage,
	CPUImage<Vec<3,float>>& colorOutput,
	const CPUBuffer<float3>& gradientA,
	const CPUBuffer<float3>& gradientB,
	const CPUBuffer<CurveSegment>& curveA,
	const CPUBuffer<CurveSegment>& curveB) {

	float iter = iterInput.at(globalID.x,globalID.y);
	const Complex& p = processedPositionImage.at(globalID.x,globalID.y);
	if(iter == MAXITER){
		INSIDE_COLORING_KERNEL(globalID,localID,iterInput,processedPositionImage,statsImage,colorOutput,gradientB,gradientA,curveB,curveA);
	}else{
		OUTSIDE_COLORING_KERNEL(globalID,localID,iterInput,processedPositionImage,statsImage,colorOutput,gradientA,gradientB,curveA,curveB);
	}
}



