#pragma once
#include "CalculationActionBase.h"
#include "../Actions/BufferGeneratorAction.h"
#include "../Actions/FunctionCallAction.h"
#include "Typedefs.h"
#include "Enums.h"

/*
 * MarianiSilverAction.h
 *
 *  Created on: 08.06.2016
 *      Author: tly
 */

struct MarianiSilverAction :
	CalculationActionBase<KV("positionBuffer",UInt2Buffer)>{

	MarianiSilverAction();
protected:
	struct Rectangle{
		uint32_t x,y,w,h;
	};
	using RectangleBuffer = Buffer<Rectangle>;

	BufferGeneratorAction<Vec<2,uint32_t>> positionBufferGenerator;
	BufferGeneratorAction<Rectangle> rectangleBufferGenerator1;
	BufferGeneratorAction<Rectangle> rectangleBufferGenerator2;
	BufferGeneratorAction<uint8_t> filterBufferGenerator;
    BufferGeneratorAction<uint32_t> atomicIndexBufferGenerator;

	FunctionCallAction<Input(KV("imageRange",Range)),KV("elemCount",uint32_t)> bufferRangeAction{
		[](const Range& r){
			return r.x * r.y;
		}
	};

	KernelGeneratorAction<FloatImage,RectangleBuffer,UCharBuffer> filterKernelGenerator;
	KernelAction<Input(
		KV("iterationImage",FloatImage),
		KV("rectangleBuffer",RectangleBuffer),
		KV("filterBuffer",UCharBuffer)
	),KernelOutput<2>> filterKernelAction;

	KernelGeneratorAction<UInt2Buffer,RectangleBuffer,RectangleBuffer,UCharBuffer,UIntBuffer> buildBufferActionGenerator;
	KernelAction<Input(
		KV("positionBuffer",UInt2Buffer),
		KV("rectangleBuffer",RectangleBuffer),
		KV("newRectangleBuffer",RectangleBuffer),
		KV("filterBuffer",UCharBuffer),
		KV("atomicIndex",UIntBuffer)
	),KernelOutput<>> buildBufferAction;

	RectangleBuffer* rectBuffer1 = nullptr, *rectBuffer2 = nullptr;

	int32_t currentStepSize = -1;
	uint32_t currentRange = 0;
	uint32_t rectangleCount = 0;

	void reset();

	bool step();
};





