#include "../Utils.h"
#include "../Multisampling.h"

kernel void reductionKernel(global read_only float* input,uint32_t w,uint32_t h,global write_only uchar4* output,uint32_t wo,uint32_t ho){
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	float3 val = { 0,0,0 };
	if(MULTISAMPLING_ENABLED){
		for(int i = 0; i < MULTISAMPLING_SIZE; i++){
			for(int j = 0; j < MULTISAMPLING_SIZE; j++){
				uint32_t index = (globalID.x * MULTISAMPLING_SIZE + i) + (globalID.y * MULTISAMPLING_SIZE + j) * w;
				val += (float3)(input[3 * index + 0],input[3 * index + 1],input[3 * index + 2]);
			}
		}
		val /= (float)(MULTISAMPLING_SIZE * MULTISAMPLING_SIZE);
	}else{
		uint32_t index = globalID.x + globalID.y * w;
		val += (float3)(input[3 * index + 0],input[3 * index + 1],input[3 * index + 2]);
	}
	uchar4 rgba = { convert_uchar3(255 * val), 255 };
	output[globalID.x + globalID.y * wo] = rgba;
}