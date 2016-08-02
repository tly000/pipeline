#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include "../Multisampling.h"
#include "../Type/Complex.h"
#include "../../Type/Vec.h"
#include <iostream>
#include <atomic>
using std::min;

/*
 * calculation.cpp
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

inline Complex f(const Complex z,const Complex c){
	return FORMULA;
}

float4 noStats(float4 current,unsigned i,Complex z){
	return current;
}

float4 expSmoothing(float4 current,unsigned i,Complex z){
	return current + (float4){expf(-tofloat(cabs2(z))),0,0,0};
}

float4 recipSmoothing(float4 current,unsigned i,Complex z){
	return current + (float4){1/(tofloat(cabs2(z)) + 1),0,0,0};
}

float4 trap1(float4 current,unsigned i,Complex z){
	if(i == 0){
		current[0] = 1000;
	}
	float l = sqrt(tofloat(cabs2(z)));
	return float4{
		std::min(current[0],fabsf(l-0.6)),0,0,0
	};
}

void doCalculation(
	const Range& position,
	CPUImage<Complex>& image,
	CPUImage<float>& iterOutput,
	CPUImage<Complex>& processedPositionImage,
	CPUImage<float4>& statsImage,
	const Type& cReal, const Type& cImag){
	const Complex juliaC = {
		cReal, cImag
	};
	const Complex c = JULIAMODE ? juliaC : image.at(position.x,position.y);
	Complex z = JULIAMODE ? image.at(position.x,position.y) : (Complex){
		floatToType(0),
		floatToType(0)
	};
	z = c;
	Complex fastZ = z;
	float4 stats{};

	unsigned i = 0;

	while(i < MAXITER && (DISABLE_BAILOUT || tofloat(cabs2(z)) <= BAILOUT)){
		z = f(z,c);
		stats = STAT_FUNCTION(stats,i,z);

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

	float iter = i;
	if(SMOOTH_MODE && iter < MAXITER){
		iter -= log(log(tofloat(cabs2(z)))/log((float)MAXITER)) / log((float)SMOOTH_EXP);
	}
	iterOutput.at(position.x,position.y) = iter;
	processedPositionImage.at(position.x,position.y) = z;
	statsImage.at(position.x,position.y) = stats;
}

extern "C" void mandelbrotKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<float>& iterOutput,
	CPUImage<Complex>& processedPositionImage, CPUImage<float4>& statsImage,
	const Type& cReal, const Type& cImag) {
	doCalculation(globalID,image,iterOutput,processedPositionImage,statsImage,cReal,cImag);
}

extern "C" void successiveRefinementKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<float>& iterOutput,
	CPUImage<Complex>& processedPositionImage, CPUImage<float4>& statsImage,
	const Type& cReal, const Type& cImag,
	const CPUBuffer<Vec<2,uint32_t>>& positionBuffer, const uint32_t& stepSize) {
	Vec<2,uint32_t> pos = positionBuffer.at(globalID.x);
	Range globalPosition = {
		pos[0],
		pos[1]
	};
	doCalculation(globalPosition,image,iterOutput,processedPositionImage,statsImage,cReal,cImag);
}

extern "C" void successiveRefinementFilter(
	const Range& globalID, const Range& localID, CPUImage<float>& iterOutput,
	CPUImage<Complex>& processedPositionInput,
	const uint32_t& stepSize,
	const CPUBuffer<Vec<2,uint32_t>>& positionBuffer, CPUBuffer<uint8_t>& filterBuffer) {
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

	Complex p[3][3] = {
		{
			processedPositionInput.at(pos[0] - stepSize,(pos[1] - stepSize)),
			processedPositionInput.at(pos[0],( pos[1] - stepSize)),
			processedPositionInput.at(pos[0] + stepSize,(pos[1] - stepSize))
		},{
			processedPositionInput.at(pos[0] - stepSize,(pos[1])),
			processedPositionInput.at(pos[0] ,(pos[1])),
			processedPositionInput.at(pos[0] + stepSize ,(pos[1]))
		},{
			processedPositionInput.at(pos[0] - stepSize ,(pos[1] + stepSize)),
			processedPositionInput.at(pos[0] ,( pos[1] + stepSize)),
			processedPositionInput.at(pos[0] + stepSize ,(pos[1] + stepSize))
		}
	};
	if(equal){
		int step = stepSize;
		for(int i = -step; i <= step; i++){
			for(int j = -step; j <= step; j++){
				iterOutput.at(i + pos[0],j + pos[1]) = fiter;

				//linear interpolation of z-values

				Vec<2,int32_t> iPos = {
					min((i + step) / step, 1), min((j + step) / step,1)
				};
				Vec<2,float> tPos = Vec<2,int32_t>{
					i + step - step * iPos[0], j + step - step * iPos[1]
				} / float(step);
				//tPos =  smoothstep(0.0f,1.0f,tPos);
				Complex result = cmix(
					cmix(p[iPos[1]][iPos[0]],p[iPos[0]][iPos[0]+1],floatToType(tPos[0])),
					cmix(p[iPos[1]+1][iPos[0]],p[iPos[1]+1][iPos[0]+1],floatToType(tPos[0])),
					floatToType(tPos[1])
				);
				processedPositionInput.at(i + pos[0], j + pos[1]) = result;
			}
		}
	}
}

extern "C" void successiveRefinementBuildPositionBuffer(
		const Range& globalID, const Range& localID,
		CPUBuffer<Vec<2,int32_t>>& positionBuffer,
		const CPUBuffer<uint8_t>& filterBuffer,
		CPUBuffer<Vec<2,int32_t>>& newPositionBuffer,
		CPUBuffer<uint32_t>& atomicIndexBuffer,
		const uint32_t& stepSize) {
	auto& atomicIndex = reinterpret_cast<std::atomic_uint&>(atomicIndexBuffer.at(0));

	if(!filterBuffer.at(globalID.x)){
		uint32_t startOffset = atomicIndex.fetch_add(12);
		auto p = positionBuffer.at(3 * globalID.x + 2);

		for(int i = 0; i < 2; i++){
			for(int j = 0; j < 2; j++){
				auto point = p - Vec<2,int32_t>{i,j} * int(stepSize);

				newPositionBuffer.at(startOffset + 6 * i + 3 * j + 0) = point + Vec<2,int32_t>{stepSize/2,0};
				newPositionBuffer.at(startOffset + 6 * i + 3 * j + 1) = point + Vec<2,int32_t>{0,stepSize/2};
				newPositionBuffer.at(startOffset + 6 * i + 3 * j + 2) = point + Vec<2,int32_t>{stepSize/2,stepSize/2};
			}
		}
	}
}

extern "C" void successiveIterationKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<float>& iterOutput,  CPUImage<Complex>& processedPositionImage, CPUImage<float4>& statsImage,
	const Type& cReal, const Type& cImag,
	const CPUBuffer<Vec<2,uint32_t>>& positionBuffer,
	CPUBuffer<uint8_t>& filterBuffer, const uint8_t& first) {

	Vec<2,uint32_t> position = positionBuffer.at(globalID.x);
	const Complex juliaC = {
		cReal, cImag
	};
	const Complex c = JULIAMODE ? juliaC : image.at(position[0],position[1]);
	Complex z = !first ? processedPositionImage.at(position[0],position[1]) :
			JULIAMODE ? image.at(position[0],position[0]) :
			(Complex){floatToType(0),floatToType(0)};
	z = c;
	Complex fastZ = z;

	float4 stats = statsImage.at(position[0],position[0]);
	if(first){
		iterOutput.at(position[0],position[1]) = 0;
		stats = {0,0,0,0};
	}

	unsigned i = 0;
	while(i < 100 && (DISABLE_BAILOUT || tofloat(cabs2(z)) <= BAILOUT)){
		z = f(z,c);
		stats = STAT_FUNCTION(stats,i,z);

		if(CYCLE_DETECTION){
			fastZ = f(f(fastZ,c),c);
			if(tofloat(cabs2(csub(fastZ,z))) < 1e-10){
				if(!CYCLE_DETECTION_VISUALIZE){
					i = 100;
				}
				break;
			}
		}
		i++;
	}

	filterBuffer.at(globalID.x) = i != 100;

	float iter = i + iterOutput.at(position[0],position[1]);
	if(i != 100 && SMOOTH_MODE && iter < MAXITER){
		iter -= log(log(tofloat(cabs2(z)))/log((float)MAXITER)) / log((float)SMOOTH_EXP);
	}
	iterOutput.at(position[0],position[1]) = min<float>(iter,MAXITER);
	processedPositionImage.at(position[0],position[1]) = z;
	statsImage.at(position[0],position[1]) = stats;
}

extern "C" void successiveIterationFill(
	const Range& globalID, const Range& localID,
	CPUImage<float>& iterOutput, const CPUBuffer<Vec<2,uint32_t>>& positionBuffer) {

	Vec<2,uint32_t> position = positionBuffer.at(globalID.x);
	iterOutput.at(position[0],position[1]) = MAXITER;
}

extern "C" void successiveIterationBuildPositionBuffer(
		const Range& globalID, const Range& localID,
		CPUBuffer<Vec<2,int32_t>>& positionBuffer,
		const CPUBuffer<uint8_t>& filterBuffer,
		CPUBuffer<Vec<2,int32_t>>& newPositionBuffer,
		CPUBuffer<uint32_t>& atomicIndexBuffer) {
	auto& atomicIndex = reinterpret_cast<std::atomic_uint&>(atomicIndexBuffer.at(0));
	if(!filterBuffer.at(globalID.x)){
		newPositionBuffer.at(atomicIndex++) = positionBuffer.at(globalID.x);
	}
}

extern "C" void marianiSilverKernel(
	const Range& globalID, const Range& localID,
	CPUImage<Complex>& image, CPUImage<float>& iterOutput,
	CPUImage<Complex>& processedPositionImage, CPUImage<float4>& statsImage,
	const Type& cReal, const Type& cImag,
	const CPUBuffer<Vec<2,uint32_t>>& positionBuffer) {
	Vec<2,uint32_t> pos = positionBuffer.at(globalID.x);
	Range globalPosition = {
		pos[0],
		pos[1]
	};
	doCalculation(globalPosition,image,iterOutput,processedPositionImage,statsImage,cReal,cImag);
}

struct Rectangle{
	int x,y,w,h;
};

extern "C" void marianiSilverFilter(
	const Range& globalID, const Range& localID,
	CPUImage<float>& iterImage,
	const CPUBuffer<Rectangle>& rectangleBuffer,
	CPUBuffer<uint8_t>& filterBuffer
) {
	const Rectangle& r = rectangleBuffer.at(globalID.x);

	bool rising[4] = {true,true,true,true};
	bool falling[4] = {true,true,true,true};
	bool outside = true, inside = true;

	float lastA = iterImage.at(r.x,r.y),
		  lastB = iterImage.at(r.x,r.y + r.h-1);
	for(int i = 1; i < r.w-1; i++){
		float a = iterImage.at(r.x+i,r.y);
		rising[0] &= a >= lastA;
		falling[0] &= a <= lastA;
		float b = iterImage.at(r.x+i,r.y+r.h-1);
		rising[1] &= b >= lastB;
		falling[1] &= b <= lastB;
		lastA = a;
		lastB = b;

		outside &= a != MAXITER;
		outside &= b != MAXITER;

		inside &= a == MAXITER;
		inside &= b == MAXITER;
	}
	lastA = iterImage.at(r.x,r.y);
	lastB = iterImage.at(r.x + r.w-1,r.y);
	for(int j = 1; j < r.h-1; j++){
		float a = iterImage.at(r.x,r.y+j);
		rising[2] &= a >= lastA;
		falling[2] &= a <= lastA;
		float b = iterImage.at(r.x+r.w-1,r.y+j);
		rising[3] &= b >= lastB;
		falling[3] &= b <= lastB;
		lastA = a;
		lastB = b;

		outside &= a != MAXITER;
		outside &= b != MAXITER;

		inside &= a == MAXITER;
		inside &= b == MAXITER;
	}
	bool equal =
		(inside || outside) &&
		(rising[0] || falling[0]) &&
		(rising[1] || falling[1]) &&
		(rising[2] || falling[2]) &&
		(rising[3] || falling[3]);

	filterBuffer.at(globalID.x) = equal;
	if(equal){
		for(int i = 1; i < r.w - 1; i++){
			for(int j = 1; j < r.h - 1; j++){
				//iterImage.at(r.x + i,r.y + j) = 0;
				float u = (float)(i)/(r.w-1);
				float v = (float)(j)/(r.h-1);

				float f0 = iterImage.at(r.x,r.y + j);
				float f1 = iterImage.at(r.x + r.w-1,r.y + j);
				float g0 = iterImage.at(r.x + i,r.y);
				float g1 = iterImage.at(r.x + i,r.y + r.h-1);
				iterImage.at(r.x + i,r.y + j) = (mix(f0,f1,u)*v*(1-v) + mix(g0,g1,v)*u*(1-u))/(u*(1-u) + v*(1-v));
				//iterImage[r.x + i + w * (r.y + j)] = mix(g0,g1,v) + mix(f0,f1,u) - mix(mix(p00,p10,u),mix(p01,p11,u),v);
			}
		}
	}
}

extern "C" void marianiSilverBuildBuffers(
		const Range& globalID, const Range& localID,
		CPUBuffer<Vec<2,int32_t>>& positionBuffer,
		const CPUBuffer<Rectangle>& rectangleBuffer,
		CPUBuffer<Rectangle>& newRectangleBuffer,
		const CPUBuffer<uint8_t>& filterBuffer,
		CPUBuffer<uint32_t>& atomicIndexBuffer) {
	auto& atomicPositionIndex = reinterpret_cast<std::atomic_uint&>(atomicIndexBuffer.at(0));
	auto& atomicRectangleIndex = reinterpret_cast<std::atomic_uint&>(atomicIndexBuffer.at(1));

	if(!filterBuffer.at(globalID.x)){
		const Rectangle& r = rectangleBuffer.at(globalID.x);
		//subdivide
		int w2 = r.w / 2, h2 = r.h/2;
		int oddX = r.w % 2, oddY = r.h % 2;
		Rectangle a = {
			r.x,r.y,w2+1,h2+1
		};
		Rectangle b = {
			r.x+w2,r.y,w2 + oddX,h2+1
		};
		Rectangle c = {
			r.x,r.y+h2,w2+1,h2 + oddY
		};
		Rectangle d = {
			r.x+w2,r.y+h2,w2 + oddX,h2 + oddY
		};
 		int index = atomicRectangleIndex.fetch_add(4);
		newRectangleBuffer.at(index + 0) = a;
		newRectangleBuffer.at(index + 1) = b;
		newRectangleBuffer.at(index + 2) = c;
		newRectangleBuffer.at(index + 3) = d;

		int posIndex = atomicPositionIndex.fetch_add(r.h-2);
		for(int i = 0; i < r.h - 2; i++){
			positionBuffer.at(posIndex + i) = {r.x + w2, r.y + i + 1};
		}
		posIndex = atomicPositionIndex.fetch_add(r.w-2);
		for(int i = 0; i < r.w - 2; i++){
			positionBuffer.at(posIndex + i) = {r.x + i + 1, r.y + h2};
		}
	}
}

