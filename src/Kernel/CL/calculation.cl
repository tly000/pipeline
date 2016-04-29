#include "../Type/StandardTypes.h"
#include "../Type/Complex.h"

#ifndef MAXITER 
#define MAXITER 64
#endif

#ifndef BAILOUT 
#define BAILOUT 4
#endif

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

kernel void mandelbrotKernel(global read_only Complex* positionInput,uint32_t w,uint32_t h,global write_only uint32_t* iterOutput,uint32_t w2,uint32_t h2){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	
	int bufferIndex = globalID.x + w * globalID.y; 

	Complex c = positionInput[bufferIndex];
	Complex z = { 
		floatToType(0), 
		floatToType(0)
	};

	unsigned i = 0;
	while(i < MAXITER && tofloat(cabs2(z)) <= BAILOUT){
		z = cadd(csqr(z),c);
		i++;
	}
	iterOutput[bufferIndex] = i;
}