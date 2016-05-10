#include "../Utils.h"
#include "../Multisampling.h"

kernel void reductionKernel(global read_only float* input,uint32_t w,uint32_t h,global write_only float* output,uint32_t wo,uint32_t ho){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	int2 localID = {
		get_local_id(0),
		get_local_id(1)
	};
	
	if(MULTISAMPLING_ENABLED){
		output[globalID.x + globalID.y * wo] = input[globalID.x + globalID.y * w];
	}else{
		output[globalID.x + globalID.y * wo] = input[globalID.x + globalID.y * w];
	}
}