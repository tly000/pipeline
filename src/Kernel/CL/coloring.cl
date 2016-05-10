#include "../Type/StandardTypes.h"

#ifndef MAXITER 
#define MAXITER 64
#endif

#ifndef BAILOUT 
#define BAILOUT 4
#endif

kernel void coloringKernel(global read_only float* iterInput,uint32_t w,uint32_t h,global write_only uint32_t* colorOutput,uint32_t w2,uint32_t h2) {
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	int bufferIndex = globalID.x + w * globalID.y;
	
	float iter = iterInput[bufferIndex];
	typedef struct{
		uint8_t r,g,b,a;
	} Color;
	Color color;
	if(iter == MAXITER){
		color = (Color){ 255, 255, 255, 255};
	}else{
		float val = fabs(fmod(iter / 100,2.0f)-1.0f);
		color = (Color){ 0, (uint8_t)(val * 150), (uint8_t)(val * 255), 255};
	}
	colorOutput[bufferIndex] = *(uint32_t*)&color;
}
