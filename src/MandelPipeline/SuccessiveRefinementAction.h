#pragma once
#include "CalculationActionBase.h"
#include "../Actions/BufferGeneratorAction.h"
#include "../Actions/FunctionCallAction.h"

/*
 * SuccessiveRefinmentAction.h
 *
 *  Created on: 02.06.2016
 *      Author: tly
 */

struct SuccessiveRefinementAction :
	CalculationActionBase<KV("positionBuffer",UInt2Buffer),KV("stepSize",uint32_t)>{

	SuccessiveRefinementAction(Factory& f);
protected:
	Factory& factory;

	BufferGeneratorAction<Vec<2,uint32_t>> positionBufferGenerator1;
	BufferGeneratorAction<Vec<2,uint32_t>> positionBufferGenerator2;
	BufferGeneratorAction<uint8_t> filterBufferGenerator;

	FunctionCallAction<Input(KV("imageRange",Range)),KV("elemCount",uint32_t)> bufferRangeAction{
		[](const Range& r){
			return r.x * r.y;
		}
	};

	KernelGeneratorAction<FloatImage,VariantComplexImage,uint32_t,UInt2Buffer,UCharBuffer> filterKernelGenerator;
	KernelAction<Input(
		KV("iterationImage",FloatImage),
		KV("processedPositionImage",VariantComplexImage),
		KV("stepSize",uint32_t),
		KV("positionBuffer",UInt2Buffer),
		KV("filterBuffer",UCharBuffer)
	),KernelOutput<3>> filterKernelAction;

	KernelGeneratorAction<UInt2Buffer,UCharBuffer,UInt2Buffer,UIntBuffer,uint32_t> buildBufferActionGenerator;
	KernelAction<Input(
		KV("positionBuffer",UInt2Buffer),
		KV("filterBuffer",UCharBuffer),
		KV("newPositionBuffer",UInt2Buffer),
		KV("atomicIndex",UIntBuffer),
		KV("stepSize",uint32_t)
	),KernelOutput<>> buildBufferAction;

	UIntBuffer atomicIndexBuffer;
	UInt2Buffer* positionBuffer1 = nullptr, *positionBuffer2 = nullptr;

	uint32_t currentRange = 0;
	uint32_t currentStepSize = 16;

	void reset();

	bool step();
};


