#pragma once
#include "CalculationActionBase.h"

/*
 * SuccessiveIterationAction.h
 *
 *  Created on: 06.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct SuccessiveIterationAction :
	CalculationActionBase<Factory,T,
		KV("positionBuffer",UInt2Buffer<Factory>),
		KV("filterBuffer",UCharBuffer<Factory>),
		KV("first",uint8_t)>{

	SuccessiveIterationAction(Factory& f, std::string typeName)
	  :CalculationActionBase<Factory,T,KV("positionBuffer",UInt2Buffer<Factory>),KV("filterBuffer",UCharBuffer<Factory>),KV("first",uint8_t)>(f,typeName),
	   factory(f),
	   finalActionGenerator(f),
	   buildBufferActionGenerator(f),
	   positionBufferGenerator1(f),
	   positionBufferGenerator2(f),
	   filterBufferGenerator(f),
	   atomicIndexBuffer(f.template createBuffer<uint32_t>(1)){
		this->kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("successiveIterationKernel");

		this->finalActionGenerator.getInput("programName"_c).setDefaultValue("calculation");
		this->finalActionGenerator.getInput("kernelName"_c).setDefaultValue("successiveIterationFill");
		this->definesAction.naturalConnect(this->finalActionGenerator);
		this->finalActionGenerator.naturalConnect(this->finalAction);

		this->buildBufferActionGenerator.getInput("programName"_c).setDefaultValue("calculation");
		this->buildBufferActionGenerator.getInput("kernelName"_c).setDefaultValue("successiveIterationBuildPositionBuffer");
		this->definesAction.naturalConnect(this->buildBufferActionGenerator);
		this->buildBufferActionGenerator.naturalConnect(this->buildBufferAction);

		this->delegateInput("imageRange"_c, bufferRangeAction.getInput("imageRange"_c));
		this->delegateInput("iterationImage"_c, finalAction.getInput("iterationImage"_c));

		this->bufferRangeAction.naturalConnect(this->positionBufferGenerator1);
		this->bufferRangeAction.naturalConnect(this->positionBufferGenerator2);
		this->bufferRangeAction.naturalConnect(this->filterBufferGenerator);

		this->filterBufferGenerator.template output<0>() >> this->kernelAction.getInput("filterBuffer"_c);
		this->filterBufferGenerator.template output<0>() >> this->buildBufferAction.getInput("filterBuffer"_c);
	}
protected:
	Factory& factory;

	KernelGeneratorAction<Factory,FloatImage<Factory>,UInt2Buffer<Factory>> finalActionGenerator;
	KernelAction<Factory,Input(
		KV("iterationImage",FloatImage<Factory>),
		KV("positionBuffer",UInt2Buffer<Factory>)
	),KernelOutput<>> finalAction;

	KernelGeneratorAction<Factory,UInt2Buffer<Factory>,UCharBuffer<Factory>,UInt2Buffer<Factory>,UIntBuffer<Factory>> buildBufferActionGenerator;
	KernelAction<Factory,Input(
		KV("positionBuffer",UInt2Buffer<Factory>),
		KV("filterBuffer",UCharBuffer<Factory>),
		KV("newPositionBuffer",UInt2Buffer<Factory>),
		KV("atomicIndex",UIntBuffer<Factory>)
	),KernelOutput<>> buildBufferAction;

	BufferGeneratorAction<Factory,Vec<2,uint32_t>> positionBufferGenerator1;
	BufferGeneratorAction<Factory,Vec<2,uint32_t>> positionBufferGenerator2;
	BufferGeneratorAction<Factory,uint8_t> filterBufferGenerator;

	UIntBuffer<Factory> atomicIndexBuffer;
	UInt2Buffer<Factory>* positionBuffer1, *positionBuffer2;

	FunctionCallAction<Input(KV("imageRange",Range)),KV("elemCount",uint32_t)> bufferRangeAction{
		[](const Range& r){
			return r.x * r.y;
		}
	};

	uint32_t currentIter = 0;
	const uint32_t iterStep = 100;
	bool decreasingPointCount = false;
	uint32_t currentRange = 0;

	void reset(){
		currentIter = 0;
		decreasingPointCount = false;
	}

	bool step(){
		Range imageRange = this->getInput("imageRange"_c).getValue();
		uint32_t maxIter = this->getInput("iterations"_c).getValue();

		do{
			if(currentIter == 0){
				std::vector<Vec<2,uint32_t>> currentPositionVector;
				currentPositionVector.reserve(imageRange.x * imageRange.y);
				for(uint32_t i = 0; i < imageRange.x; i++){
					for(uint32_t j = 0; j < imageRange.y; j++){
						currentPositionVector.push_back({i,j});
					}
				}

				currentRange = currentPositionVector.size();

				this->positionBufferGenerator1.run();
				this->positionBufferGenerator2.run();
				this->positionBuffer1 = &this->positionBufferGenerator1.template getOutput<0>().getValue();
				this->positionBuffer2 = &this->positionBufferGenerator2.template getOutput<0>().getValue();
				this->positionBuffer1->copyFromBuffer(currentPositionVector,0,currentRange);
			}

			this->kernelAction.getInput("positionBuffer"_c).setDefaultValue(*this->positionBuffer1);
			this->kernelAction.getInput("first"_c).setDefaultValue(currentIter == 0);
			this->kernelAction.getInput("globalSize"_c).setDefaultValue(Range{currentRange,1,1});
			this->kernelAction.run();

			this->atomicIndexBuffer.copyFromBuffer({0},0,1);
			this->buildBufferAction.getInput("positionBuffer"_c).setDefaultValue(*this->positionBuffer1);
			this->buildBufferAction.getInput("newPositionBuffer"_c).setDefaultValue(*this->positionBuffer2);
			this->buildBufferAction.getInput("atomicIndex"_c).setDefaultValue(this->atomicIndexBuffer);
			this->buildBufferAction.getInput("globalSize"_c).setDefaultValue(Range{currentRange,1,1});
			this->buildBufferAction.run();

			std::vector<uint32_t> atomicIndexVector;
			this->atomicIndexBuffer.copyToBuffer(atomicIndexVector);
			uint32_t newRange = atomicIndexVector.front();

			if(!decreasingPointCount){
				decreasingPointCount = currentRange > newRange;
			}else if(decreasingPointCount && currentRange - newRange < 10){
				this->finalAction.getInput("positionBuffer"_c).setDefaultValue(*this->positionBuffer1);
				this->finalAction.getInput("globalSize"_c).setDefaultValue(Range{newRange,1,1});
				this->finalAction.run();
				return true;
			}

			std::swap(this->positionBuffer1,this->positionBuffer2);
			this->currentRange = newRange;
			this->currentIter += 100;
		}while(!decreasingPointCount);

		return this->currentIter >= maxIter;
	}
};

