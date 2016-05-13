#include "../Type/StandardTypes.h"

#ifndef MAXITER 
#define MAXITER 64
#endif

#ifndef BAILOUT 
#define BAILOUT 4
#endif

//see opencl 6.1.5 sizeof(float3)
kernel void coloringKernel(global read_only float* iterInput,uint32_t w,uint32_t h,global write_only float* colorOutput,uint32_t w2,uint32_t h2) {
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	int bufferIndex = globalID.x + globalID.y * w;
	
	float iter = iterInput[bufferIndex];
	if(iter == MAXITER){
		float3 color = { 1,1,1 };
		colorOutput[3 * bufferIndex + 0] = color.s0;
		colorOutput[3 * bufferIndex + 1] = color.s1;
		colorOutput[3 * bufferIndex + 2] = color.s2;
	}else{
		float val = fabs(fmod(iter / 100,2)-1);
		float3 color = { 0, val * 0.7f , val };
		colorOutput[3 * bufferIndex + 0] = color.s0;
		colorOutput[3 * bufferIndex + 1] = color.s1;
		colorOutput[3 * bufferIndex + 2] = color.s2;
	}
}
