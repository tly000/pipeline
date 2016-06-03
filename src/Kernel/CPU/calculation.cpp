#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include "../Multisampling.h"
#include "../Type/Complex.h"
#include <iostream>

/*
 * calculation.cpp
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

#ifndef MAXITER
#define MAXITER 64
#endif

#ifndef BAILOUT
#define BAILOUT 4
#endif

inline Complex f(const Complex z,const Complex c){
	return FORMULA;
}

void doCalculation(const Range& position, CPUImage<Complex>& image, CPUImage<float>& iterOutput,const Type& cReal, const Type& cImag){
	const Complex juliaC = {
		cReal, cImag
	};
	const Complex c = JULIAMODE ? juliaC : image.at(position.x,position.y);
	Complex z = JULIAMODE ? image.at(position.x,position.y) : (Complex){
		floatToType(0),
		floatToType(0)
	};
	Complex fastZ = z;

	unsigned i = 0;

	float l = 0;

	while(i < MAXITER && (l = tofloat(cabs2(z))) <= BAILOUT){
		z = f(z,c);
		if(CYCLE_DETECTION){
			fastZ = f(f(fastZ,c),c);
			if(tofloat(cabs2(csub(fastZ,z))) < 1e-10){
				if(!CYCLE_DETECTION_VISUALIZE){
					i = MAXITER;
				}
				break;
			}
		}
		i++;
	}

	iterOutput.at(position.x,position.y) = i;
}

extern "C" void mandelbrotKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<float>& iterOutput,const Type& cReal, const Type& cImag) {
	doCalculation(globalID,image,iterOutput,cReal,cImag);
}

extern "C" void successiveRefinementKernelBase(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<float>& iterOutput,const Type& cReal, const Type& cImag, const uint32_t& stepSize) {
	Range globalPosition = {
		globalID.x * stepSize,
		globalID.y * stepSize,
		0
	};
	doCalculation(globalPosition,image,iterOutput,cReal,cImag);
}

extern "C" void successiveRefinementKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<float>& iterOutput,
	const Type& cReal, const Type& cImag,
	const CPUBuffer<Vec<2,uint32_t>>& positionBuffer) {
	Vec<2,uint32_t> pos = positionBuffer.at(globalID.x);
	Range globalPosition = {
		pos[0],
		pos[1]
	};
	doCalculation(globalPosition,image,iterOutput,cReal,cImag);
}

extern "C" void successiveRefinementFilter(
	const Range& globalID, const Range& localID, const CPUImage<float>& iterOutput, const uint32_t& stepSize,
	const CPUBuffer<Vec<2,uint32_t>>& positionBuffer, CPUBuffer<uint32_t>& filterBuffer) {
	Vec<2,uint32_t> pos = positionBuffer.at(3 * globalID.x + 2);

	float fiter = iterOutput.at(pos[0],pos[1]);
	int iter = fiter;
	bool equal =
		iter == iterOutput.at(pos[0] - stepSize,pos[1] - stepSize) &&
		iter == iterOutput.at(pos[0] + stepSize,pos[1] - stepSize) &&
		iter == iterOutput.at(pos[0] - stepSize,pos[1] + stepSize) &&
		iter == iterOutput.at(pos[0] + stepSize,pos[1] + stepSize) &&
		iter == iterOutput.at(pos[0] - stepSize,pos[1]) &&
		iter == iterOutput.at(pos[0] + stepSize,pos[1]) &&
		iter == iterOutput.at(pos[0], pos[1] - stepSize) &&
		iter == iterOutput.at(pos[0], pos[1] + stepSize);

	filterBuffer.at(globalID.x) = equal;
}

