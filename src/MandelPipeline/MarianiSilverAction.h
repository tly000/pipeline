#pragma once
#include "CalculationActionBase.h"
#include "../Actions/BufferGeneratorAction.h"

/*
 * MarianiSilverAction.h
 *
 *  Created on: 08.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct MarianiSilverAction :
	CalculationActionBase<Factory,T,KV("positionBuffer",UInt2Buffer<Factory>)>{

	MarianiSilverAction(Factory& f, std::string typeName)
	  :CalculationActionBase<Factory,T,KV("positionBuffer",UInt2Buffer<Factory>)>(f,typeName),
	   factory(f),
	   positionBufferGenerator(f),
	   rectangleBufferGenerator1(f),
	   rectangleBufferGenerator2(f),
	   filterBufferGenerator(f),
	   filterKernelGenerator(f),
	   buildBufferActionGenerator(f),
	   atomicIndexBuffer(f.template createBuffer<uint32_t>(2)){
		this->kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("marianiSilverKernel");

		this->definesAction.naturalConnect(this->filterKernelGenerator);
		this->filterKernelGenerator.getInput("kernelName"_c).setDefaultValue("marianiSilverFilter");
		this->filterKernelGenerator.getInput("programName"_c).setDefaultValue("calculation");
		this->filterKernelGenerator.naturalConnect(filterKernelAction);

		this->buildBufferActionGenerator.getInput("programName"_c).setDefaultValue("calculation");
		this->buildBufferActionGenerator.getInput("kernelName"_c).setDefaultValue("marianiSilverBuildBuffers");
		this->definesAction.naturalConnect(this->buildBufferActionGenerator);
		this->buildBufferActionGenerator.naturalConnect(this->buildBufferAction);

		this->delegateInput("imageRange"_c, bufferRangeAction.getInput("imageRange"_c));
		this->delegateInput("iterationImage"_c,filterKernelAction.getInput("iterationImage"_c));

		this->bufferRangeAction.naturalConnect(this->positionBufferGenerator);
		this->bufferRangeAction.naturalConnect(this->rectangleBufferGenerator1);
		this->bufferRangeAction.naturalConnect(this->rectangleBufferGenerator2);
		this->bufferRangeAction.naturalConnect(this->filterBufferGenerator);

		this->filterBufferGenerator.template output<0>() >> this->filterKernelAction.getInput("filterBuffer"_c);
		this->filterBufferGenerator.template output<0>() >> this->buildBufferAction.getInput("filterBuffer"_c);
		this->positionBufferGenerator.template output<0>() >> this->buildBufferAction.getInput("positionBuffer"_c);
		this->positionBufferGenerator.template output<0>() >> this->kernelAction.getInput("positionBuffer"_c);
	}
protected:
	Factory& factory;

	struct Rectangle{
		int x,y,w,h;
	};
	using RectangleBuffer = typename Factory::template Buffer<Rectangle>;

	BufferGeneratorAction<Factory,Vec<2,uint32_t>> positionBufferGenerator;
	BufferGeneratorAction<Factory,Rectangle> rectangleBufferGenerator1;
	BufferGeneratorAction<Factory,Rectangle> rectangleBufferGenerator2;
	BufferGeneratorAction<Factory,uint8_t> filterBufferGenerator;

	FunctionCallAction<Input(KV("imageRange",Range)),KV("elemCount",uint32_t)> bufferRangeAction{
		[](const Range& r){
			return r.x * r.y;
		}
	};

	KernelGeneratorAction<Factory,FloatImage<Factory>,RectangleBuffer,UCharBuffer<Factory>> filterKernelGenerator;
	KernelAction<Factory,Input(
		KV("iterationImage",FloatImage<Factory>),
		KV("rectangleBuffer",RectangleBuffer),
		KV("filterBuffer",UCharBuffer<Factory>)
	),KernelOutput<2>> filterKernelAction;

	KernelGeneratorAction<Factory,UInt2Buffer<Factory>,RectangleBuffer,RectangleBuffer,UCharBuffer<Factory>,UIntBuffer<Factory>> buildBufferActionGenerator;
	KernelAction<Factory,Input(
		KV("positionBuffer",UInt2Buffer<Factory>),
		KV("rectangleBuffer",RectangleBuffer),
		KV("newRectangleBuffer",RectangleBuffer),
		KV("filterBuffer",UCharBuffer<Factory>),
		KV("atomicIndex",UIntBuffer<Factory>)
	),KernelOutput<>> buildBufferAction;

	UIntBuffer<Factory> atomicIndexBuffer;
	RectangleBuffer* rectBuffer1 = nullptr, *rectBuffer2 = nullptr;

	int32_t currentStepSize = -1;
	uint32_t currentRange = 0;
	uint32_t rectangleCount = 0;

	void reset(){
		currentStepSize = -1;
		currentRange = 0;
	}

	bool step(){
		Range imageRange = this->getInput("imageRange"_c).getValue();
		if(currentStepSize == -1){
			this->currentStepSize = min(imageRange.x,imageRange.y);
			std::vector<Vec<2,uint32_t>> currentPositionVector;
			currentPositionVector.reserve(2 * (imageRange.x + imageRange.y));
			for(uint32_t i = 0; i < imageRange.x; i ++){
				currentPositionVector.push_back({i,0});
				currentPositionVector.push_back({i,imageRange.y-1});
			}
			for(uint32_t i = 1; i < imageRange.y-1; i ++){
				currentPositionVector.push_back({0,i});
				currentPositionVector.push_back({imageRange.x-1,i});
			}
			this->positionBufferGenerator.run();
			this->rectangleBufferGenerator1.run();
			this->rectangleBufferGenerator2.run();

			this->rectangleCount = 1;
			this->currentRange = currentPositionVector.size();

			this->rectBuffer1 = &this->rectangleBufferGenerator1.template getOutput<0>().getValue();
			this->rectBuffer2 = &this->rectangleBufferGenerator2.template getOutput<0>().getValue();
			auto& positionBuffer = this->positionBufferGenerator.template getOutput<0>().getValue();
			positionBuffer.copyFromBuffer(currentPositionVector,0,currentRange);
			std::vector<Rectangle> rectangles{{0,0,imageRange.x,imageRange.y}};
			this->rectBuffer1->copyFromBuffer(rectangles,0,1);
		}

		if(this->currentStepSize == 2){
			std::vector<Rectangle> remainingRects;
			this->rectBuffer2->copyToBuffer(remainingRects);
			std::vector<Vec<2,uint32_t>> positionVector;
			for(const auto& r : remainingRects){
				for(int i = 1; i < r.w-1; i++){
					for(int j = 1; j < r.h-1; j++){
						positionVector.push_back({r.x + i, r.y + j});
					}
				}
			}

			auto& positionBuffer = this->positionBufferGenerator.template getOutput<0>().getValue();
			positionBuffer.copyFromBuffer(positionVector,0,positionVector.size());

			this->kernelAction.getInput("globalSize"_c).setDefaultValue(Range{positionVector.size(),1,1});
			this->kernelAction.run();
			return true;
		}else{
			this->kernelAction.getInput("globalSize"_c).setDefaultValue(Range{this->currentRange,1,1});
			this->kernelAction.run();

			this->filterKernelAction.getInput("rectangleBuffer"_c).setDefaultValue(*this->rectBuffer1);
			this->filterKernelAction.getInput("globalSize"_c).setDefaultValue(Range{this->rectangleCount,1,1});
			this->filterKernelAction.run();

			this->atomicIndexBuffer.copyFromBuffer({0,0},0,2);
			this->buildBufferAction.getInput("rectangleBuffer"_c).setDefaultValue(*this->rectBuffer1);
			this->buildBufferAction.getInput("newRectangleBuffer"_c).setDefaultValue(*this->rectBuffer2);
			this->buildBufferAction.getInput("atomicIndex"_c).setDefaultValue(this->atomicIndexBuffer);
			this->buildBufferAction.getInput("globalSize"_c).setDefaultValue(Range{this->rectangleCount,1,1});
			this->buildBufferAction.run();

			std::vector<uint32_t> atomicIndexVector;
			this->atomicIndexBuffer.copyToBuffer(atomicIndexVector);
			std::swap(this->rectBuffer1,this->rectBuffer2);
			this->currentRange = atomicIndexVector.at(0);
			this->rectangleCount = atomicIndexVector.at(1);
			this->currentStepSize /= 2;
			return this->rectangleCount == 0;
		}
	}
};





