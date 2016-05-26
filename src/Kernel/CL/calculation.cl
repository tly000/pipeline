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

kernel void mandelbrotKernel(
	global read_only Complex* positionInput,uint32_t w,uint32_t h,
	global write_only float* iterOutput,uint32_t w2,uint32_t h2,const Type cReal,const Type cImag){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	
	int bufferIndex = globalID.x + w * globalID.y; 

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