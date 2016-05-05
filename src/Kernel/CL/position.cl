#include "../Type/Complex.h"

kernel void positionKernel(global write_only Complex* output,uint32_t w,uint32_t h, Type offsetReal, Type offsetImag, Type scale){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	Complex pos = {
		floatToType(2 * ((float)(globalID.x) - w/2) / w),
		floatToType(2 * ((float)(globalID.y) - h/2) / h * (float)h/w)
	};
	Complex scaleFactor ={
		scale, floatToType(0)
	};
	Complex offset ={
		offsetReal, offsetImag
	};
	output[globalID.x + globalID.y * w] = cadd(cmul(scaleFactor,pos),offset);
}