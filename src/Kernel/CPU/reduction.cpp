#include "../../Type/Range.h"
#include "../../Platform/CPU/CPUImage.h"
#include "../Utils.h"
#include "../Multisampling.h"
#include "../../Type/Vec.h"
#include <iostream>
#include <cmath>

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
				int xpos = globalID.x * MULTISAMPLING_SIZE + i;
				int ypos = globalID.y * MULTISAMPLING_SIZE + j;
				val += input.at(xpos,ypos);
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

//unused gauss filter code
//inline int clip(int x,int w){
//	return x < 0 ? 0 : x >= w ? w-1 : x;
//}
//
//struct Mask{
//	float kernel[3*MULTISAMPLING_SIZE][3*MULTISAMPLING_SIZE];
//};
//
//inline constexpr Mask createGaussMask(){
//	Mask m{};
//	constexpr float mu = 1.5*MULTISAMPLING_SIZE;
//	constexpr float sigma = 0.3*MULTISAMPLING_SIZE;
//	constexpr float sqrt2pi = 2.50662827463100050;
//
//	float weights = 0;
//	for(int i = 0; i < 3*MULTISAMPLING_SIZE; i++){
//		for(int j = 0; j < 3*MULTISAMPLING_SIZE; j++){
//			float lSquared = (i-mu) * (i-mu) + (j-mu) * (j-mu);
//			m.kernel[i][j] = 1/(sigma * sqrt2pi) * std::exp(-lSquared/(2 * sigma * sigma));
//			weights += m.kernel[i][j];
//		}
//	}
//	for(int i = 0; i < 3*MULTISAMPLING_SIZE; i++){
//		for(int j = 0; j < 3*MULTISAMPLING_SIZE; j++){
//			float lSquared = (i-mu) * (i-mu) + (j-mu) * (j-mu);
//			m.kernel[i][j] /= weights;
//		}
//	}
//	return m;
//}
//
//extern "C" void reductionKernel(const Range& globalID,const Range& localID,const CPUImage<Vec<3,float>>& input,CPUImage<unsigned>& output){
//	Vec<3,float> val{ 0,0,0 };
//	static constexpr Mask gaussMask = createGaussMask();
//
//	if(MULTISAMPLING_ENABLED){
//		for(int i = 0; i < 3*MULTISAMPLING_SIZE; i++){
//			for(int j = 0; j < 3*MULTISAMPLING_SIZE; j++){
//				int xpos = clip(globalID.x * MULTISAMPLING_SIZE + i,input.getWidth());
//				int ypos = clip(globalID.y * MULTISAMPLING_SIZE + j,input.getHeight());
//				val += gaussMask.kernel[i][j] * input.at(xpos,ypos);
//			}
//		}
//		//val /= (MULTISAMPLING_SIZE * MULTISAMPLING_SIZE);
//	}else{
//		val = input.at(globalID.x,globalID.y);
//	}
//	Vec<4,uint8_t> rgba(255 * val);
//	rgba[3] = 255; //making the color opaque
//	output.at(globalID.x,globalID.y) = reinterpret_cast<uint32_t&>(rgba);
//}
//
