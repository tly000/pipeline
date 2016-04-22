#include "../Type/StandardTypes.h"

#ifndef MAXITER 
#define MAXITER 64
#endif

#ifndef BAILOUT 
#define BAILOUT 4
#endif

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

kernel void coloringKernel(global read_only uint32_t* iterInput,uint32_t w,uint32_t h,global write_only uint32_t* colorOutput,uint32_t w2,uint32_t h2) {
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	int bufferIndex = globalID.x + w * globalID.y;
	float val = sqrt(((float)iterInput[bufferIndex]) / MAXITER);
	struct{
		uint8_t r,g,b,a;
	} color = { 0, (uint8_t)(val * 255), (uint8_t)((1-val) * 255), 255};
	colorOutput[bufferIndex] = *(uint32_t*)&color;
}
