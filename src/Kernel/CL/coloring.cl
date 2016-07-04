#include "../Type/StandardTypes.h"
#include "../Type/Complex.h"
#include "../Type/CurveSegment.h"

float3 getFloat3(const global float* p,uint i){
	return (float3)(p[3*i+0],p[3*i+1],p[3*i+2]);
}

float3 gradientSample(const global float* gradient,uint32_t gradientSize,float t){
	t *= gradientSize - 1;
	float fT = fmod(t,1);
	int iT = floor(t);
	
	if(iT >= gradientSize - 1){
		return getFloat3(gradient,gradientSize- 1);
	}else{
		return (1-fT) * getFloat3(gradient,iT) + fT * getFloat3(gradient,iT+1);
	}
}

void iterationGradient(
	int2 globalID,
	const global float* iterInput,
	const global Complex* processedPositionImage,
	global float4* statsImage,
	global float* colorOutput,
	uint32_t w,uint32_t h,
	const global float* gradientA, uint32_t aSize,
	const global float* gradientB, uint32_t bSize,
	const global CurveSegment* curveA,uint32_t curveASize,
	const global CurveSegment* curveB,uint32_t surveBSize) {
	uint32_t bufferIndex = globalID.x + w * globalID.y;
	float iter = iterInput[bufferIndex];
	float t = curveSample(curveA,curveASize,iter / (float)MAXITER);
	float3 color = gradientSample(gradientA,aSize,t);
	colorOutput[3 * bufferIndex + 0] = color.x;
	colorOutput[3 * bufferIndex + 1] = color.y;
	colorOutput[3 * bufferIndex + 2] = color.z;
}

void angleGradient(
	int2 globalID,
	const global float* iterInput,
	const global Complex* processedPositionImage,
	global float4* statsImage,
	global float* colorOutput,
	uint32_t w,uint32_t h,
	const global float* gradientA, uint32_t aSize,
	const global float* gradientB, uint32_t bSize,
	const global CurveSegment* curveA,uint32_t curveASize,
	const global CurveSegment* curveB,uint32_t surveBSize) {
	
	uint32_t bufferIndex = globalID.x + w * globalID.y;
	Complex p = processedPositionImage[bufferIndex];
	float angle = atan2(tofloat(p.imag),tofloat(p.real));
	float t = curveSample(curveA,curveASize,(angle+M_PI)/(2*M_PI));
	float3 color = gradientSample(gradientA,aSize,t);
	colorOutput[3 * bufferIndex + 0] = color.x;  
	colorOutput[3 * bufferIndex + 1] = color.y;
	colorOutput[3 * bufferIndex + 2] = color.z; 
}

void flatColor(
	int2 globalID,
	const global float* iterInput,
	const global Complex* processedPositionImage,
	global float4* statsImage,
	global float* colorOutput,
	uint32_t w,uint32_t h,
	const global float* gradientA, uint32_t aSize,
	const global float* gradientB, uint32_t bSize,
	const global CurveSegment* curveA,uint32_t curveASize,
	const global CurveSegment* curveB,uint32_t surveBSize) {
	
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
	global float4* statsImage,
	global float* colorOutput,
	uint32_t w,uint32_t h,
	const global float* gradientA, uint32_t aSize,
	const global float* gradientB, uint32_t bSize,
	const global CurveSegment* curveA,uint32_t curveASize,
	const global CurveSegment* curveB,uint32_t surveBSize) {
		
	uint32_t bufferIndex = globalID.x + w * globalID.y;
	Complex p = processedPositionImage[bufferIndex];
	float length = sqrt(tofloat(cabs2(p))) / sqrt(BAILOUT);
	float t = curveSample(curveA,curveASize,length > 1 ? 1/length : length);
	float3 color = gradientSample(gradientA,aSize,t);
	colorOutput[3 * bufferIndex + 0] = color.x;  
	colorOutput[3 * bufferIndex + 1] = color.y;
	colorOutput[3 * bufferIndex + 2] = color.z;
}

//see opencl 6.1.5 sizeof(float3)
kernel void coloringKernel(
  global float* iterInput,uint32_t w,uint32_t h,
  global Complex* processedPositionImage,uint32_t w2,uint32_t h2,
  global float4* statsImage,uint32_t w4,uint32_t h4,
  global float* colorOutput,uint32_t w3,uint32_t h3,
  const global float* gradientA,uint32_t aSize,
  const global float* gradientB,uint32_t bSize,
  const global CurveSegment* curveA,uint32_t curveASize,
  const global CurveSegment* curveB,uint32_t curveBSize) {
	int2 globalID = {
		get_global_id(0),
		get_global_id(1)
	};
	int bufferIndex = globalID.x + globalID.y * w;
	
	float iter = iterInput[bufferIndex];
	Complex p = processedPositionImage[bufferIndex];
	
	if(iter == MAXITER){
		INSIDE_COLORING_KERNEL(globalID,
			iterInput,
			processedPositionImage,
			statsImage,
			colorOutput,w,h,
			gradientB,bSize,gradientA,aSize,
			curveB,curveBSize,curveA,curveASize);
	}else{
		OUTSIDE_COLORING_KERNEL(globalID,
			iterInput,
			processedPositionImage,
			statsImage,
			colorOutput,w,h,
			gradientA,aSize,gradientB,bSize,
			curveA,curveASize,curveB,curveBSize);	
	}
}

