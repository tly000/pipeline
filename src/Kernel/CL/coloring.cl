#include "../Type/StandardTypes.h"
#include "../Type/Complex.h"

float3 gradientSample(const global float* gradient,uint32_t gradientSize,float t){
	t *= gradientSize - 1;
	float fT = fmod(t,1);
	int iT = floor(t);

	if(iT >= gradientSize - 1){
		return gradient[gradientSize- 1];
	}else{
		return (1-fT) * gradient[iT] + fT * gradient[iT+1];
	}
}

void iterationGradient(
	int2 globalID,
	const global float* iterInput,
	const global Complex* processedPositionImage,
	global float* colorOutput,
	uint32_t w,uint32_t h,
	const global float* gradientA, uint32_t aSize,
	const global float* gradientB, uint32_t bSize) {
	uint32_t bufferIndex = globalID.x + w * globalID.y;
	float iter = iterInput[bufferIndex];
	float3 color = gradientSample(gradientA,aSize,iter / float(MAXITER));
	colorOutput[3 * bufferIndex + 0] = color.x;
	colorOutput[3 * bufferIndex + 1] = color.y;
	colorOutput[3 * bufferIndex + 2] = color.z;
}

void angleGradient(
	int2 globalID,
	const global float* iterInput,
	const global Complex* processedPositionImage,
	global float* colorOutput,
	uint32_t w,uint32_t h,
	const global float* gradientA, uint32_t aSize,
	const global float* gradientB, uint32_t bSize) {
	
	uint32_t bufferIndex = globalID.x + w * globalID.y;
	Complex p = processedPositionImage[bufferIndex];
	float angle = atan2(tofloat(p.imag),tofloat(p.real));
	float3 color = gradientSample(gradientA,aSize,fmod((angle+M_PI)/(2*M_PI),1));
	colorOutput[3 * bufferIndex + 0] = color.x;  
	colorOutput[3 * bufferIndex + 1] = color.y;
	colorOutput[3 * bufferIndex + 2] = color.z; 
}

void flatColor(
	int2 globalID,
	const global float* iterInput,
	const global Complex* processedPositionImage,
	global float* colorOutput,
	uint32_t w,uint32_t h,
	const global float* gradientA, uint32_t aSize,
	const global float* gradientB, uint32_t bSize) {
	
	uint32_t bufferIndex = globalID.x + w * globalID.y;
	float3 color = gradientSample(gradientA,aSize,0);
	colorOutput[3 * bufferIndex + 0] = color.x;  
	colorOutput[3 * bufferIndex + 1] = color.y;
	colorOutput[3 * bufferIndex + 2] = color.z; 
}

void lengthGradient(
	int2 globalID,
	const global float* iterInput,
	const global Complex* processedPositionImage,
	global float* colorOutput,
	uint32_t w,uint32_t h,
	const global float* gradientA, uint32_t aSize,
	const global float* gradientB, uint32_t bSize) {
		
	uint32_t bufferIndex = globalID.x + w * globalID.y;
	Complex p = processedPositionImage[bufferIndex];
	float length = sqrt(tofloat(cabs2(p))) / sqrt(BAILOUT);
	float3 color = gradientSample(gradientA,aSize,length > 1 ? 1/length : length);
	colorOutput[3 * bufferIndex + 0] = color.x;  
	colorOutput[3 * bufferIndex + 1] = color.y;
	colorOutput[3 * bufferIndex + 2] = color.z;
}

void angleMixGradient(
	int2 globalID,
	const global float* iterInput,
	const global Complex* processedPositionImage,
	global float* colorOutput,
	uint32_t w,uint32_t h,
	const global float* gradientA, uint32_t aSize,
	const global float* gradientB, uint32_t bSize) {

	uint32_t bufferIndex = globalID.x + w * globalID.y;
	Complex p = processedPositionImage[bufferIndex];
	float length = sqrt(tofloat(cabs2(p))) / sqrt(BAILOUT);	
	float angle = atan2(tofloat(p.imag),tofloat(p.real));
	float3 color = mix(
		gradientSample(gradientA,aSize,fmod((angle+M_PI)/(2*M_PI),1)),
		gradientSample(gradientB,bSize,bSize-1),
		length > 1 ? 1/length : length
	);
	
	colorOutput[3 * bufferIndex + 0] = color.x;  
	colorOutput[3 * bufferIndex + 1] = color.y;
	colorOutput[3 * bufferIndex + 2] = color.z;
}

//see opencl 6.1.5 sizeof(float3)
kernel void coloringKernel(
  global float* iterInput,uint32_t w,uint32_t h,
  global Complex* processedPositionImage,uint32_t w2,uint32_t h2,
  global float* colorOutput,uint32_t w3,uint32_t h3,
  global float* gradientA,uint32_t aSize,
  global float* gradientB,uint32_t bSize) {
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	int bufferIndex = globalID.x + globalID.y * w;
	
	global float* iter = iterInput + bufferIndex;
	Complex p = processedPositionImage[bufferIndex];
	
	if(SMOOTH_MODE && *iter != MAXITER){
		*iter += 1 - log(log(tofloat(cabs2(p)))/log((float)MAXITER)) / log((float)SMOOTH_EXP);
	}
	if(*iter == MAXITER){
		INSIDE_COLORING_KERNEL(globalID,
			iterInput,
			processedPositionImage,
			colorOutput,w,h,
			gradientB,bSize,gradientA,aSize);
	}else{
		OUTSIDE_COLORING_KERNEL(globalID,
			iterInput,
			processedPositionImage,
			colorOutput,w,h,
			gradientA,aSize,gradientB,bSize);	
	}
}
