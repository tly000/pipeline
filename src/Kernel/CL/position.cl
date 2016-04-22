#include "../Type/Complex.h"

kernel void positionKernel(global write_only Complex* output,uint32_t w,uint32_t h){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	output[globalID.x + globalID.y * w] = (Complex){
		fromfloat(3 * ((float)(globalID.x) - w/2) / w),
		fromfloat(3 * ((float)(globalID.y) - h/2) / h)
	};
}