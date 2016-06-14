#include "../Type/StandardTypes.h"
#include "../Type/Complex.h"

//see opencl 6.1.5 sizeof(float3)
kernel void coloringKernel(
  global read_only float* iterInput,uint32_t w,uint32_t h,
  global read_only Complex* processedPositionImage,uint32_t w2,uint32_t h2,
  global write_only float* colorOutput,uint32_t w3,uint32_t h3) {
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	int bufferIndex = globalID.x + globalID.y * w;
	
	float iter = iterInput[bufferIndex];
	Complex p = processedPositionImage[bufferIndex];
	
	if(SMOOTH_MODE && iter != MAXITER){
		iter -= log(log(tofloat(cabs2(p)))/log((float)MAXITER)) / log((float)SMOOTH_EXP);
	}
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
