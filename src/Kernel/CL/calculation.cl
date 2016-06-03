#include "../Type/StandardTypes.h"
#include "../Type/Complex.h"
#include "../Multisampling.h"

#ifndef MAXITER 
#define MAXITER 64
#endif

#ifndef BAILOUT 
#define BAILOUT 4
#endif

inline Complex f(const Complex z,const Complex c){
	return FORMULA;
}

void doCalculation(const int2 position,global read_only Complex* positionInput,uint32_t w,uint32_t h,
	global write_only float* iterOutput,uint32_t w2,uint32_t h2,const Type cReal,const Type cImag){
	
	int bufferIndex = position.x + w * position.y; 

	const Complex juliaC = {
		cReal, cImag
	};
	const Complex c = JULIAMODE ? juliaC : positionInput[bufferIndex];
	Complex z = JULIAMODE ? positionInput[bufferIndex] : (Complex){ 
		floatToType(0), 
		floatToType(0)
	};
	Complex fastZ = z;

	unsigned i = 0;
	while(i < MAXITER && tofloat(cabs2(z)) <= BAILOUT){
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
	iterOutput[bufferIndex] = i;
}

kernel void mandelbrotKernel(
	global read_only Complex* positionInput,uint32_t w,uint32_t h,
	global write_only float* iterOutput,uint32_t w2,uint32_t h2,const Type cReal,const Type cImag){
	int2 globalPosition = { 
		get_global_id(0), 
		get_global_id(1)
	};
	doCalculation(globalPosition, positionInput, w,h, iterOutput, w2, h2, cReal,cImag);
}

kernel void successiveRefinementKernel(
	global Complex* positionInput,uint32_t w,uint32_t h,
	global float* iterOutput,uint32_t w2,uint32_t h2,const Type cReal,const Type cImag,
	global int2* positionBuffer) {
	doCalculation(positionBuffer[get_global_id(0)],positionInput,w,h,iterOutput,w,h,cReal,cImag);
}

kernel void successiveRefinementFilter(
	global float* iterOutput, int w, int h, uint stepSize,
	global uint2* positionBuffer, global uchar* filterBuffer) {
	uint2 pos = positionBuffer[3 * get_global_id(0) + 2];
	float fiter = iterOutput[pos.x + w * pos.y];
	int iter = fiter;
	bool equal =
		iter == iterOutput[pos.x - stepSize + w * (pos.y - stepSize)] &&
		iter == iterOutput[pos.x + stepSize + w * (pos.y - stepSize)] &&
		iter == iterOutput[pos.x - stepSize + w * (pos.y + stepSize)] &&
		iter == iterOutput[pos.x + stepSize + w * (pos.y + stepSize)] &&
		iter == iterOutput[pos.x - stepSize + w * (pos.y)] &&
		iter == iterOutput[pos.x + stepSize + w * (pos.y)] &&
		iter == iterOutput[pos.x + w * ( pos.y - stepSize)] &&
		iter == iterOutput[pos.x + w * ( pos.y + stepSize)];

	filterBuffer[get_global_id(0)] = equal;
	if(equal){
		for(int i = -stepSize; i < (int)(stepSize); i++){
			for(int j = -stepSize; j < (int)(stepSize); j++){
				iterOutput[i + pos[0] + w * (j + pos[1])] = fiter;
			}
		}
	}
}
