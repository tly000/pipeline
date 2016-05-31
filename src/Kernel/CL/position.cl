#include "../Type/Complex.h"
#include "../Utils.h"
#include "../Multisampling.h"

kernel void positionKernel(global write_only Complex* output,uint32_t w,uint32_t h, Type offsetReal, Type offsetImag, Complex scaleFactor){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	Complex offset ={
		offsetReal, offsetImag
	};
	float fx = globalID.x, fy = globalID.y;
	if(MULTISAMPLING_ENABLED){
		uint64_t seed = globalID.x + globalID.y;

		if(MULTISAMPLING_PATTERN == JITTERING){
			float xR = (uint32_t)(seed = random(seed)) / (float)UINT_MAX;
			float yR = (uint32_t)(seed = random(seed)) / (float)UINT_MAX;
			fx += xR - 0.5;
			fy += yR - 0.5;
		}
	}
	Complex pos = {
		floatToType(2 * (fx - w/2) / w),
		floatToType(2 * (fy - h/2) / h * (float)h/w)
	};
	output[globalID.x + globalID.y * w] = cadd(cmul(scaleFactor,pos),offset);
}