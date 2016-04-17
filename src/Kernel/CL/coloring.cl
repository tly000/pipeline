#include "../Type/StandardTypes.h"

#ifndef MAXITER 
#define MAXITER 64
#endif

#ifndef BAILOUT 
#define BAILOUT 4
#endif

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

kernel void coloringKernel(read_only image2d_t iterInput, write_only image2d_t colorOutput) {
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	float val = sqrt(((float)read_imageui(iterInput,sampler,globalID).x) / MAXITER);
	struct{
		uint8_t r,g,b,a;
	} color = { 0, (uint8_t)(val * 255), (uint8_t)((1-val) * 255), 255};
	write_imageui(colorOutput,globalID,(uint4)(*((uint32_t*)(&color)),0,0,0));
}
