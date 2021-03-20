#include "../Type/StandardTypes.h"
#include "../Type/Complex.h"
#include "../Multisampling.h"

inline Complex f(const Complex z,const Complex c){
	return FORMULA;
}

float4 noStats(float4 current,unsigned i,Complex z){
	return current;
}

float4 expSmoothing(float4 current,unsigned i,Complex z){
	return current + (float4){exp(-tofloat(cabs2(z))),0,0,0};
}

float4 recipSmoothing(float4 current,unsigned i,Complex z){
	return current + (float4){1/(tofloat(cabs2(z)) + 1),0,0,0};
}

float4 trap1(float4 current,unsigned i,Complex z){
	if(i == 0){
		current.x = 1000;
	}
	float l = sqrt(tofloat(cabs2(z)));
	return (float4){
		min(current.x,fabs(l-0.6f)),0,0,0
	};
}

//https://www.shadertoy.com/view/4dfGRn
float4 orbitTraps(float4 current, unsigned i, Complex z){
	float2 zf = {
		tofloat(z.real), tofloat(z.imag)
	};
	return min(
		current, 
		(float4){
			fabs(0.0+zf.y + 0.5*sin(zf.x)), 
			fabs(1.0+zf.x + 0.5*sin(zf.y)), 
			dot(zf,zf),
			length(zf-floor(zf)-0.5f)
		}
	);
}

void doCalculation(const int2 position,
	global Complex* positionInput,uint32_t w,uint32_t h,
	global float* iterOutput,uint32_t w2,uint32_t h2,
	global Complex* processedPositionOutput,uint32_t w3,uint32_t h3,
	global float4* statsOutput,uint32_t w4,uint32_t h4,
	const Type cReal,const Type cImag){
	
	int bufferIndex = position.x + w * position.y; 

	const Complex juliaC = {
		cReal, cImag
	};
	const Complex c = JULIAMODE ? juliaC : positionInput[bufferIndex];
	Complex z = JULIAMODE ? positionInput[bufferIndex] : (Complex){ 
		floatToType(0), 
		floatToType(0)
	};
	Complex fastZ = z;

	float4 stats = {0,0,0,0};
	unsigned i = 0;
	while(i < MAXITER && (DISABLE_BAILOUT || tofloat(cabs2(z)) <= BAILOUT)){
		z = f(z,c);
		stats = STAT_FUNCTION(stats, i, z);
		
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
	float iter = i;
	if(SMOOTH_MODE && i != MAXITER){
		iter -= log(log(tofloat(cabs2(z)))/log((float)MAXITER)) / log((float)SMOOTH_EXP);
	}
	
	iterOutput[bufferIndex] = iter;
	processedPositionOutput[bufferIndex] = z;
	statsOutput[bufferIndex] = stats;
}

kernel void mandelbrotKernel(
	global Complex* positionInput,uint32_t w,uint32_t h,
	global float* iterOutput,uint32_t w2,uint32_t h2,
	global Complex* processedPositionOutput,uint32_t w3,uint32_t h3,
	global float4* statsOutput,uint32_t w4,uint32_t h4,
	const Type cReal,const Type cImag){
	int2 globalPosition = { 
		get_global_id(0), 
		get_global_id(1)
	};
	doCalculation(globalPosition, 
		positionInput, w,h, 
		iterOutput, w2, h2,
		processedPositionOutput,w3,h3,
		statsOutput,w4,h4,
		cReal,cImag
	);
}

kernel void successiveRefinementKernel(
	global Complex* positionInput,uint32_t w,uint32_t h,
	global float* iterOutput,uint32_t w2,uint32_t h2,
	global Complex* processedPositionOutput,uint32_t w3,uint32_t h3,
	global float4* statsOutput,uint32_t w4,uint32_t h4,
	const Type cReal,const Type cImag,
	global int2* positionBuffer, uint32_t bufferSize, 
	uint32_t stepSize) {
	int2 pos = positionBuffer[get_global_id(0)];
	doCalculation(pos,
		positionInput,w,h,
		iterOutput,w,h,
		processedPositionOutput,w,h,
		statsOutput,w,h,
		cReal,cImag
	);
}

kernel void successiveRefinementFilter(
	global float* iterOutput, int w, int h,
	global Complex* processedPositionInput, int w2, int h2, 
	uint stepSize,
	global uint2* positionBuffer, uint32_t bufferSize, 
	global uchar* filterBuffer, uint32_t bufferSize2) {
	uint2 pos = positionBuffer[3 * get_global_id(0) + 2];
	float fiter = iterOutput[pos.x + w * pos.y];
	int iter = fiter;
	bool equal =
		iter == iterOutput[pos.x - stepSize + w * (pos.y - stepSize)] &&
		iter == iterOutput[pos.x + stepSize + w * (pos.y - stepSize)] &&
		iter == iterOutput[pos.x - stepSize + w * (pos.y + stepSize)] &&
		iter == iterOutput[pos.x + stepSize + w * (pos.y + stepSize)] &&
		iter == iterOutput[pos.x - stepSize + w * (pos.y)] &&
		iter == iterOutput[pos.x + stepSize + w * (pos.y)] &&
		iter == iterOutput[pos.x + w * ( pos.y - stepSize)] &&
		iter == iterOutput[pos.x + w * ( pos.y + stepSize)];

	filterBuffer[get_global_id(0)] = equal;
	Complex p[3][3] = {
		{
			processedPositionInput[pos.x - stepSize + w * (pos.y - stepSize)],
			processedPositionInput[pos.x + w * ( pos.y - stepSize)],
			processedPositionInput[pos.x + stepSize + w * (pos.y - stepSize)]
		},{
			processedPositionInput[pos.x - stepSize + w * (pos.y)],
			processedPositionInput[pos.x + w * (pos.y)],
			processedPositionInput[pos.x + stepSize + w * (pos.y)]
		},{
			processedPositionInput[pos.x - stepSize + w * (pos.y + stepSize)],
			processedPositionInput[pos.x + w * ( pos.y + stepSize)],
			processedPositionInput[pos.x + stepSize + w * (pos.y + stepSize)]
		}
	};
	
	if(equal){
		int step = stepSize;
		for(int i = -step; i <= step; i++){
			for(int j = -step; j <= step; j++){
				iterOutput[i + pos.x + w * (j + pos.y)] = fiter;
//				if(!(abs(i) % step == 0 && abs(j) % step == 0) ){
//					iterOutput[i + pos.x + w * (j + pos.y)] = 0;
//				}
				//linear interpolation of z-values
				
				uint2 iPos = min((uint2)(i + step, j + step) / step, (uint2)(1,1));
				float2 tPos = (float2)(i + step - step * iPos.x, j + step - step * iPos.y) / (float)step;
				//tPos =  smoothstep(0.0f,1.0f,tPos);
				Complex result = cmix(
					cmix(p[iPos.y][iPos.x],p[iPos.y][iPos.x+1],floatToType(tPos.x)),
					cmix(p[iPos.y+1][iPos.x],p[iPos.y+1][iPos.x+1],floatToType(tPos.x)),
					floatToType(tPos.y)
				);
				processedPositionInput[i + pos.x + w * (j + pos.y)] = result;
			}
		}
	}
}

kernel void successiveRefinementBuildPositionBuffer(
		global int2* positionBuffer, uint32_t s1, 
		global uchar* filterBuffer, uint32_t s2,
		global int2* newPositionBuffer, uint32_t s3,
		global uint* atomicIndex, uint32_t s4,
		uint32_t stepSize) {
	
	local uint localSize;
	if(get_local_id(0) == 0){
		localSize = 0;
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	int offset = -1;
	if(!filterBuffer[get_global_id(0)]){
		offset = atomic_inc(&localSize);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
		
	local uint localIndex;
	if(get_local_id(0) == 0){
		localIndex = atomic_add(atomicIndex,12 * localSize);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	
	if(offset != -1){
		int startOffset = localIndex + 12 * offset;
		int2 p = positionBuffer[3 * get_global_id(0) + 2];
		
		for(int i = 0; i < 2; i++){
			for(int j = 0; j < 2; j++){
				int2 point = p - (int2)(i,j) * (int)stepSize;
				
				newPositionBuffer[startOffset + 6 * i + 3 * j + 0] = point + (int2)(stepSize/2,0);
				newPositionBuffer[startOffset + 6 * i + 3 * j + 1] = point + (int2)(0,stepSize/2);
				newPositionBuffer[startOffset + 6 * i + 3 * j + 2] = point + (int2)(stepSize/2,stepSize/2);
			}
		}
	}
}

kernel void successiveIterationKernel(
		global Complex* positionInput,uint32_t w,uint32_t h,
		global float* iterOutput,uint32_t w2,uint32_t h2,
		global Complex* processedPositionOutput,uint32_t w3,uint32_t h3,
		global float4* statsOutput,uint32_t w4,uint32_t h4,
		const Type cReal,const Type cImag, 
		global int2* positionBuffer, uint32_t s1, 
		global uchar* filterBuffer, uint32_t s2,
		uchar first) {
	
	int2 position = positionBuffer[get_global_id(0)];
	int bufferIndex = position.x + w * position.y; 

	const Complex juliaC = {
		cReal, cImag
	};
	const Complex c = JULIAMODE ? juliaC : positionInput[bufferIndex];
	Complex z = 
		!first ? processedPositionOutput[bufferIndex] :
		JULIAMODE ? positionInput[bufferIndex] :
		(Complex){floatToType(0),floatToType(0)};
	Complex fastZ = z;

	float4 stats = statsOutput[bufferIndex];
	if(first){
		iterOutput[bufferIndex] = 0;
		stats = (float4){0,0,0,0};
	}
	
	unsigned i = 0;
	while(i < 100 && (DISABLE_BAILOUT || tofloat(cabs2(z)) <= BAILOUT)){
		z = f(z,c);
		stats = STAT_FUNCTION(stats, i, z);
		
		if(CYCLE_DETECTION){
			fastZ = f(f(fastZ,c),c);
			if(tofloat(cabs2(csub(fastZ,z))) < 1e-10){
				if(!CYCLE_DETECTION_VISUALIZE){
					i = 100;
				}
				break;
			}
		}
		i++;
	}
	
	filterBuffer[get_global_id(0)] = i != 100;
	if(i != 100){
		float iter = i + iterOutput[bufferIndex];
		if(SMOOTH_MODE && iter != MAXITER){
			iter -= log(log(tofloat(cabs2(z)))/log((float)MAXITER)) / log((float)SMOOTH_EXP);
		}
		iterOutput[bufferIndex] = iter;
	}else{
		iterOutput[bufferIndex] = min(i + iterOutput[bufferIndex], (float)MAXITER);
	}
	processedPositionOutput[bufferIndex] = z;
	statsOutput[bufferIndex] = stats;
}

kernel void marianiSilverKernel(
	global Complex* image,uint32_t w,uint32_t h,
	global float* iterOutput,uint32_t w2,uint32_t h2,
	global Complex* processedPositionImage,uint32_t w3,uint32_t h3,
	global float4* statsImage,uint32_t w4,uint32_t h4,
	const Type cReal, const Type cImag,
	global int2* positionBuffer,uint32_t size) {
	doCalculation(positionBuffer[get_global_id(0)],image,w,h,iterOutput,w,h,processedPositionImage,w,h,statsImage,w,h,cReal,cImag);
}

kernel void successiveIterationFill(
	global float* iterOutput,uint32_t w,uint32_t h,
	global const uint2* positionBuffer,uint32_t s) {

	uint2 position = positionBuffer[get_global_id(0)];
	iterOutput[position.x + w * position.y] = MAXITER;
}

kernel void successiveIterationBuildPositionBuffer(
		global int2* positionBuffer, uint32_t s1, 
		global uchar* filterBuffer, uint32_t s2,
		global int2* newPositionBuffer, uint32_t s3,
		global uint* atomicIndex, uint32_t s4) {
	
	local uint localSize;
	if(get_local_id(0) == 0){
		localSize = 0;
	}
	barrier(CLK_LOCAL_MEM_FENCE);

	int offset = -1;
	if(!filterBuffer[get_global_id(0)]){
		offset = atomic_inc(&localSize);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
		
	local uint localIndex;
	if(get_local_id(0) == 0){
		localIndex = atomic_add(atomicIndex,localSize);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	
	if(offset != -1){
		int startOffset = localIndex + offset;
		newPositionBuffer[startOffset] = positionBuffer[get_global_id(0)];
	}
}

typedef struct{
	int x,y,w,h;
} Rectangle;

kernel void marianiSilverFilter(
	global float* iterImage,uint32_t w,uint32_t h,
	global Rectangle* rectangleBuffer,uint32_t s,
	global uint8_t* filterBuffer,uint32_t s2
) {
	Rectangle r = rectangleBuffer[get_global_id(0)];

	bool rising[4] = {true,true,true,true};
	bool falling[4] = {true,true,true,true};
	bool outside = true, inside = true;
	
	float lastA = iterImage[r.x + w * r.y],
		  lastB = iterImage[r.x + w * (r.y + r.h-1)];
//	for(int i = 1; i < r.w-1; i++){
//		float a = iterImage[r.x+i + w * r.y];
//		rising[0] &= a >= lastA;
//		falling[0] &= a <= lastA;
//		float b = iterImage[r.x+i + w * (r.y+r.h-1)];
//		rising[1] &= b >= lastB;
//		falling[1] &= b <= lastB;
//		lastA = a;
//		lastB = b;
//		
//		outside &= a != MAXITER;
//		outside &= b != MAXITER;
//		
//		inside &= a == MAXITER;
//		inside &= b == MAXITER;
//	}
//	lastA = iterImage[r.x + w * r.y];
//	lastB = iterImage[r.x + r.w-1 + w * r.y];
//	for(int j = 1; j < r.h-1; j++){
//		float a = iterImage[r.x + w * (r.y+j)];
//		rising[2] &= a >= lastA;
//		falling[2] &= a <= lastA;
//		float b = iterImage[r.x+r.w-1 + w * (r.y+j)];
//		rising[3] &= b >= lastB;
//		falling[3] &= b <= lastB;
//		lastA = a;
//		lastB = b;
//		
//		outside &= a != MAXITER;
//		outside &= b != MAXITER;
//		
//		inside &= a == MAXITER;		
//		inside &= b == MAXITER;
//	}
	for(int i = 1; i < r.w-1; i++){
		float a = iterImage[r.x+i + w * r.y];
		rising[0] &= a >= lastA;
		falling[0] &= a <= lastA;
		float b = iterImage[r.x+i + w * (r.y+r.h-1)];
		rising[1] &= b >= lastB;
		falling[1] &= b <= lastB;
		lastA = a;
		lastB = b;
		
		outside &= a == lastA;
		outside &= b == lastA;
		
		inside &= a == MAXITER;
		inside &= b == MAXITER;
	}
	lastA = iterImage[r.x + w * r.y];
	lastB = iterImage[r.x + r.w-1 + w * r.y];
	for(int j = 1; j < r.h-1; j++){
		float a = iterImage[r.x + w * (r.y+j)];
		rising[2] &= a >= lastA;
		falling[2] &= a <= lastA;
		float b = iterImage[r.x+r.w-1 + w * (r.y+j)];
		rising[3] &= b >= lastB;
		falling[3] &= b <= lastB;
		lastA = a;
		lastB = b;
		
		outside &= a == lastA;
		outside &= b == lastA;
		
		inside &= a == MAXITER;		
		inside &= b == MAXITER;
	}
	bool equal = 
		(outside || inside);// &&
		//(rising[0] || falling[0]) &&
		//(rising[1] || falling[1]) && 
		//(rising[2] || falling[2]) && 
		//(rising[3] || falling[3]);

	filterBuffer[get_global_id(0)] = equal;
	if(equal){
		float p00 = iterImage[r.x + w * r.y];
		float p01 = iterImage[r.x + w * (r.y + r.h-1)];
		float p10 = iterImage[r.x + r.w-1 + w * r.y];
		float p11 = iterImage[r.x + r.w-1 + w * (r.y + r.h-1)];

		for(int i = 1; i < r.w - 1; i++){
			for(int j = 1; j < r.h - 1; j++){
				float u = (float)(i)/(r.w-1);
				float v = (float)(j)/(r.h-1);			

				float f0 = iterImage[r.x + w * (r.y + j)];
				float f1 = iterImage[r.x + r.w-1 + w * (r.y + j)];
				float g0 = iterImage[r.x + i + w * r.y];
				float g1 = iterImage[r.x + i + w * (r.y + r.h-1)];
				iterImage[r.x + i + w * (r.y + j)] = 0;
				//iterImage[r.x + i + w * (r.y + j)] = (mix(f0,f1,u)*v*(1-v) + mix(g0,g1,v)*u*(1-u))/(u*(1-u) + v*(1-v));
				//iterImage[r.x + i + w * (r.y + j)] = mix(g0,g1,v) + mix(f0,f1,u) - mix(mix(p00,p10,u),mix(p01,p11,u),v);
			}
		}
	}
}

kernel void marianiSilverBuildBuffers(
		global int2* positionBuffer,uint32_t positionSize,
		const global Rectangle* rectangleBuffer,uint32_t rS,
		global Rectangle* newRectangleBuffer,uint32_t rS2,
		const global uint8_t* filterBuffer,uint32_t fS,
		global uint32_t* atomicIndexBuffer,uint32_t aS) {
	global uint32_t* atomicPositionIndex = atomicIndexBuffer + 0;
	global uint32_t* atomicRectangleIndex = atomicIndexBuffer + 1;

	uint id = get_global_id(0);
	if(!filterBuffer[id]){
		const Rectangle r = rectangleBuffer[id];
		//subdivide
		int w2 = r.w / 2, h2 = r.h/2;
		int oddX = r.w % 2 ? 1 : 0, oddY = r.h % 2 ? 1 : 0;
		Rectangle a = {
			r.x,r.y,w2+1,h2+1
		};
		Rectangle b = {
			r.x+w2,r.y,w2 + oddX,h2+1
		};
		Rectangle c = {
			r.x,r.y+h2,w2+1,h2 + oddY
		};
		Rectangle d = {
			r.x+w2,r.y+h2,w2 + oddX,h2 + oddY
		};
 		int index = atomic_add(atomicRectangleIndex,4);
		newRectangleBuffer[index + 0] = a;
		newRectangleBuffer[index + 1] = b;
		newRectangleBuffer[index + 2] = c;
		newRectangleBuffer[index + 3] = d;

		int posIndex = atomic_add(atomicPositionIndex,r.h-2);
		for(int i = 0; i < r.h - 2; i++){
			positionBuffer[posIndex + i] = (int2){r.x + w2, r.y + i + 1};
		}
		posIndex = atomic_add(atomicPositionIndex,r.w-2);
		for(int i = 0; i < r.w - 2; i++){
			positionBuffer[posIndex + i] = (int2){r.x + i + 1, r.y + h2};
		}
	}
}
