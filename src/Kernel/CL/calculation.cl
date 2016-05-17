#include "../Type/StandardTypes.h"
#include "../Type/Complex.h"
#include "../Multisampling.h"

#ifndef MAXITER 
#define MAXITER 64
#endif

#ifndef BAILOUT 
#define BAILOUT 4
#endif

kernel void mandelbrotKernel(global read_only Complex* positionInput,uint32_t w,uint32_t h,global write_only float* iterOutput,uint32_t w2,uint32_t h2){
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
		z = FORMULA;
		i++;
	}
	iterOutput[bufferIndex] = i;
}