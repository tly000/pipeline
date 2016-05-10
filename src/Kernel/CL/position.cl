#include "../Type/Complex.h"
#include "../Utils.h"
#include "../Multisampling.h"

kernel void positionKernel(global write_only Complex* output,uint32_t w,uint32_t h, Type offsetReal, Type offsetImag, Type scale){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	int2 localID = {
		get_local_id(0),
		get_local_id(1)
	};
	
	Complex scaleFactor = {
		scale, floatToType(0)
	};
	Complex offset = {
		offsetReal, offsetImag
	};
	if(MULTISAMPLING_ENABLED){		
		float2 floatPos = {
			globalID.x,
			globalID.y
		};
		uint64_t seed = globalID.x + globalID.y + localID.x + localID.y;
		
		switch(MULTISAMPLING_PATTERN){
		case MULTISAMPLING_JITTERGRID:{
			float xR = (uint32_t)(seed = random(seed)) / (float)UINT_MAX;
			float yR = (uint32_t)(seed = random(seed)) / (float)UINT_MAX;
			floatPos.x += (localID.x + xR - 0.5) / w;
			floatPos.y += (localID.y + yR - 0.5) / h;	
			break;
		}
		//case MULTISAMPLING_GAUSSIAN:
		case MULTISAMPLING_UNIFORMGRID:
		default:
			floatPos.x += localID.x / w;
			floatPos.y += localID.y / h;
			break;
		}

		Complex pos = {
			floatToType(2 * ((float)(floatPos.x) - w/2) / w),
			floatToType(2 * ((float)(floatPos.y) - h/2) / h * (float)h/w)
		};

		output[
			globalID.x * MULTISAMPLING_SIZE + localID.x + 
		   (globalID.y * MULTISAMPLING_SIZE + localID.y) * w] = cadd(cmul(scaleFactor,pos),offset);
	}else{
		Complex pos = {
			floatToType(2 * ((float)(globalID.x) - w/2) / w),
			floatToType(2 * ((float)(globalID.y) - h/2) / h * (float)h/w)
		};
		output[globalID.x + globalID.y * w] = cadd(cmul(scaleFactor,pos),offset);
	}

}