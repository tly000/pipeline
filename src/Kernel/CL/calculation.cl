#include "../Type/StandardTypes.h"
#include "../Type/Complex.h"

#ifndef MAXITER 
#define MAXITER 64
#endif

#ifndef BAILOUT 
#define BAILOUT 4
#endif

const sampler_t sampler = CLK_NORMALIZED_COORDS_FALSE | CLK_ADDRESS_NONE | CLK_FILTER_NEAREST;

kernel void mandelbrotKernel(read_only image2d_t positionInput,write_only image2d_t iterOutput){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};

	float2 cF = read_imagef(positionInput,sampler,globalID).xy;
	Complex c = {cF.x,cF.y};
	Complex z = {0, 0};

	unsigned i = 0;
	while(i < MAXITER && tofloat(cabs2(z)) <= BAILOUT){
		z = cadd(cmul(z,z),c);
		i++;
	}

	write_imageui(iterOutput,globalID,(uint4)(i,0,0,0));
}