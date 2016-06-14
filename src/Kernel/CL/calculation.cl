#include "../Type/StandardTypes.h"
#include "../Type/Complex.h"
#include "../Multisampling.h"

inline Complex f(const Complex z,const Complex c){
	return FORMULA;
}

void doCalculation(const int2 position,
	global Complex* positionInput,uint32_t w,uint32_t h,
	global float* iterOutput,uint32_t w2,uint32_t h2,
	global Complex* processedPositionOutput,uint32_t w3,uint32_t h3,
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

	unsigned i = 0;
	while(i < MAXITER && (DISABLE_BAILOUT || tofloat(cabs2(z)) <= BAILOUT)){
		z = f(z,c);
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
	iterOutput[bufferIndex] = i;
	processedPositionOutput[bufferIndex] = z;
}

kernel void mandelbrotKernel(
	global read_only Complex* positionInput,uint32_t w,uint32_t h,
	global write_only float* iterOutput,uint32_t w2,uint32_t h2,
	global Complex* processedPositionOutput,uint32_t w3,uint32_t h3,
	const Type cReal,const Type cImag){
	int2 globalPosition = { 
		get_global_id(0), 
		get_global_id(1)
	};
	doCalculation(globalPosition, 
		positionInput, w,h, 
		iterOutput, w2, h2,
		processedPositionOutput,w3,h3,
		cReal,cImag
	);
}

kernel void successiveRefinementKernel(
	global Complex* positionInput,uint32_t w,uint32_t h,
	global float* iterOutput,uint32_t w2,uint32_t h2,
	global Complex* processedPositionOutput,uint32_t w3,uint32_t h3,
	const Type cReal,const Type cImag,
	global int2* positionBuffer, uint32_t stepSize) {
	int2 pos = positionBuffer[get_global_id(0)];
	doCalculation(pos,
		positionInput,w,h,
		iterOutput,w,h,
		processedPositionOutput,w,h,
		cReal,cImag
	);
}

kernel void successiveRefinementFilter(
	global float* iterOutput, int w, int h,
	global Complex* processedPositionInput, int w2, int h2, 
	uint stepSize,
	global uint2* positionBuffer, global uchar* filterBuffer) {
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
		global int2* positionBuffer, global uchar* filterBuffer,
		global int2* newPositionBuffer, global uint* atomicIndex, uint stepSize) {
	
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
		const Type cReal,const Type cImag, 
		global int2* positionBuffer, global uchar* filterBuffer, uchar first) {
	
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

	if(first){
		iterOutput[bufferIndex] = 0;
	}
	
	unsigned i = 0;
	while(i < 100 && (DISABLE_BAILOUT || tofloat(cabs2(z)) <= BAILOUT)){
		z = f(z,c);
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
	iterOutput[bufferIndex] = min(i + iterOutput[bufferIndex], (float)MAXITER);
	processedPositionOutput[bufferIndex] = z;
}

kernel void successiveIterationFill(
		global float* iterOutput,uint32_t w,uint32_t h,
		global int2* positionBuffer) {
	
	int2 position = positionBuffer[get_global_id(0)];
	int bufferIndex = position.x + w * position.y; 
	iterOutput[bufferIndex] = MAXITER;
}

kernel void successiveIterationBuildPositionBuffer(
		global int2* positionBuffer, global uchar* filterBuffer,
		global int2* newPositionBuffer, global uint* atomicIndex) {
	
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
		newPositionBuffer[localIndex + offset] = positionBuffer[get_global_id(0)];
	}
}

/*kernel void marianiSilverKernel(
	global Complex* positionInput,uint32_t w,uint32_t h,
	global float* iterOutput,uint32_t w2,uint32_t h2,
	global Complex* processedPositionOutput,uint32_t w3,uint32_t h3,
	const Type cReal,const Type cImag,
	global int2* positionBuffer) {
	int2 pos = positionBuffer[get_global_id(0)];
	doCalculation(pos,
		positionInput,w,h,
		iterOutput,w,h,
		processedPositionOutput,w,h,
		cReal,cImag
	);
}

kernel void marianiSilverFilter(
	global float* iterOutput,uint32_t w,uint32_t h,
	global uint2* rectangleBuffer, global uchar* filterBuffer) {	
	
	uint rectangleIndex = get_global_id(2);
	uint sideIndex = get_global_id(1);
	uint posIndex = get_global_id(0);
	
	uint4 rectangle = rectangleBuffer[rectangleIndex];
	uint2 offset = {
		sideIndex % 2 ? (sideIndex & 2 ? 0 : get_local_size(0)) : posIndex,
		sideIndex % 2 ? posIndex : (sideIndex & 2 ? 0 : get_local_size(0)),
	};
	
	float iter = iterOutput[rectangle.x + offset.x + w * (rectangle.y + offset.y)];
	float compareIter = iterOutput[rectangle.x + w * rectangle.y];
	
	int localIndex = get_local_size(0) * sideIndex + posIndex;
	if(localIndex == 0){
		atomic_xchg(filterBuffer + rectangleIndex, true);
	}
	barrier(CLK_GLOBAL_MEM_FENCE);
	atomic_and(filterBuffer + rectangleIndex, compareIter == iter);
}

kernel void marianiSilverBuildBuffers(
	global int2* positionBuffer, global uint2* rectangleBuffer, global uint2* newRectangleBuffer, 
	global uchar* filterBuffer, global uint* atomicIndex, uint stepSize) {	
	
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
		
	local uint localPosIndex;
	local uint localRectIndex;
	if(get_local_id(0) == 0){
		localPosIndex = atomic_add(atomicIndex + 0	,4 * (stepSize - 3) * localSize);
		localRectIndex = atomic_add(atomicIndex + 1	,4 * localSize);
	}
	barrier(CLK_LOCAL_MEM_FENCE);
	
	if(offset != -1){
		int posStartOffset = localPosIndex + 4 * (stepSize - 3) * offset;
		int2 p = rectangleBuffer[get_global_id(0)];
		
		int halfSize = stepSize/2; 
		for(int i = 0; i < halfSize - 2; i++){
			positionBuffer[startOffset + 8 * i + 0] = p + (uint2)(halfSize-1, i + 1);
			positionBuffer[startOffset + 8 * i + 1] = p + (uint2)(halfSize, i + 1);
			
			positionBuffer[startOffset + 8 * i + 2] = p + (uint2)(i + 1, halfSize-1);
			positionBuffer[startOffset + 8 * i + 3] = p + (uint2)(i + 1, halfSize);
			
			positionBuffer[startOffset + 8 * i + 4] = p + (uint2)(halfSize-1, halfSize + i + 1);
			positionBuffer[startOffset + 8 * i + 5] = p + (uint2)(halfSize, halfSize + i + 1);
			
			positionBuffer[startOffset + 8 * i + 6] = p + (uint2)(halfSize + i + 1, halfSize-1);
			positionBuffer[startOffset + 8 * i + 7] = p + (uint2)(halfSize + i + 1, halfSize);
		}
		
		int rectStartOffset = localRectIndex + 4 * offset;
		for(int i = 0; i < 2; i++){
			for(int j = 0; j < 2; j++){
				newRectangleBuffer[localRectIndex + 2 * i + j] = p + (int2)(i,j) * halfSize;
			}
		}
	}
}*/
