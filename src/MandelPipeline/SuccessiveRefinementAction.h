#pragma once
#include "CalculationActionBase.h"
#include "../Actions/BufferGeneratorAction.h"

/*
 * SuccessiveRefinmentAction.h
 *
 *  Created on: 02.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct SuccessiveRefinementAction :
	CalculationActionBase<Factory,T,KV("positionBuffer",UInt2Buffer<Factory>),KV("stepSize",uint32_t)>{

	SuccessiveRefinementAction(Factory& f, std::string typeName)
	  :CalculationActionBase<Factory,T,KV("positionBuffer",UInt2Buffer<Factory>),KV("stepSize",uint32_t)>(f,typeName),
	   factory(f),
	   positionBufferGenerator1(f),
	   positionBufferGenerator2(f),
	   filterBufferGenerator(f),
	   filterKernelGenerator(f),
	   buildBufferActionGenerator(f),
	   atomicIndexBuffer(f.template createBuffer<uint32_t>(1)){
		this->kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("successiveRefinementKernel");

		this->definesAction.naturalConnect(this->filterKernelGenerator);
		this->filterKernelGenerator.getInput("kernelName"_c).setDefaultValue("successiveRefinementFilter");
		this->filterKernelGenerator.getInput("programName"_c).setDefaultValue("calculation");
		this->filterKernelGenerator.naturalConnect(filterKernelAction);

		this->buildBufferActionGenerator.getInput("programName"_c).setDefaultValue("calculation");
		this->buildBufferActionGenerator.getInput("kernelName"_c).setDefaultValue("successiveRefinementBuildPositionBuffer");
		this->definesAction.naturalConnect(this->buildBufferActionGenerator);
		this->buildBufferActionGenerator.naturalConnect(this->buildBufferAction);

		this->delegateInput("imageRange"_c, bufferRangeAction.getInput("imageRange"_c));
		this->delegateInput("iterationImage"_c,filterKernelAction.getInput("iterationImage"_c));

		this->bufferRangeAction.naturalConnect(this->positionBufferGenerator1);
		this->bufferRangeAction.naturalConnect(this->positionBufferGenerator2);
		this->bufferRangeAction.naturalConnect(this->filterBufferGenerator);

		this->filterBufferGenerator.template output<0>() >> this->filterKernelAction.getInput("filterBuffer"_c);
		this->filterBufferGenerator.template output<0>() >> this->buildBufferAction.getInput("filterBuffer"_c);
	}
protected:
	Factory& factory;

	BufferGeneratorAction<Factory,Vec<2,uint32_t>> positionBufferGenerator1;
	BufferGeneratorAction<Factory,Vec<2,uint32_t>> positionBufferGenerator2;
	BufferGeneratorAction<Factory,uint8_t> filterBufferGenerator;

	FunctionCallAction<Input(KV("imageRange",Range)),KV("elemCount",uint32_t)> bufferRangeAction{
		[](const Range& r){
			return r.x * r.y;
		}
	};

	KernelGeneratorAction<Factory,FloatImage<Factory>,uint32_t,UInt2Buffer<Factory>,UCharBuffer<Factory>> filterKernelGenerator;
	KernelAction<Factory,Input(
		KV("iterationImage",FloatImage<Factory>),
		KV("stepSize",uint32_t),
		KV("positionBuffer",UInt2Buffer<Factory>),
		KV("filterBuffer",UCharBuffer<Factory>)
	),KernelOutput<3>> filterKernelAction;

	KernelGeneratorAction<Factory,UInt2Buffer<Factory>,UCharBuffer<Factory>,UInt2Buffer<Factory>,UIntBuffer<Factory>,uint32_t> buildBufferActionGenerator;
	KernelAction<Factory,Input(
		KV("positionBuffer",UInt2Buffer<Factory>),
		KV("filterBuffer",UCharBuffer<Factory>),
		KV("newPositionBuffer",UInt2Buffer<Factory>),
		KV("atomicIndex",UIntBuffer<Factory>),
		KV("stepSize",uint32_t)
	),KernelOutput<>> buildBufferAction;

	UIntBuffer<Factory> atomicIndexBuffer;
	UInt2Buffer<Factory>* positionBuffer1 = nullptr, *positionBuffer2 = nullptr;

	uint32_t currentRange = 0;
	uint32_t currentStepSize = 16;

	void reset(){
		currentStepSize = 16;
		currentRange = 0;
	}

	bool step(){
		Range imageRange = this->getInput("imageRange"_c).getValue();
		uint32_t filterRange = 0;
		if(currentStepSize == 16){
			std::vector<Vec<2,uint32_t>> currentPositionVector;
			currentPositionVector.reserve(imageRange.x / 16 * imageRange.y/16);
			for(uint32_t i = 0; i < imageRange.x - 32; i += 32){
				for(uint32_t j = 0; j < imageRange.y - 32; j += 32){
					currentPositionVector.push_back({i + 16,j});
					currentPositionVector.push_back({i ,j + 16});
					currentPositionVector.push_back({i + 16,j + 16});
				}
			}
			filterRange = currentPositionVector.size() / 3;
			for(uint32_t i = 0; i < imageRange.x - 32; i+= 32){
				for(uint32_t j = 0; j < imageRange.y - 32; j+= 32){
					currentPositionVector.push_back({i,j});
				}
			}

			auto restStartPoint = currentPositionVector.back();
			for(uint32_t i = restStartPoint[0] + 32; i < imageRange.x; i++){
				for(uint32_t j = 0; j < restStartPoint[1] + 32; j++){
					currentPositionVector.push_back({i,j});
				}
			}
			for(uint32_t i = 0; i < imageRange.x; i++){
				for(uint32_t j = restStartPoint[1] + 32; j < imageRange.y; j++){
					currentPositionVector.push_back({i,j});
				}
			}

			currentRange = currentPositionVector.size();
			this->positionBufferGenerator1.run();
			this->positionBufferGenerator2.run();
			this->positionBuffer1 = &this->positionBufferGenerator1.template getOutput<0>().getValue();
			this->positionBuffer2 = &this->positionBufferGenerator2.template getOutput<0>().getValue();
			this->positionBuffer1->copyFromBuffer(currentPositionVector,0,currentRange);
		}else{
			filterRange = currentRange / 3;
		}

		this->kernelAction.getInput("positionBuffer"_c).setDefaultValue(*this->positionBuffer1);
		this->kernelAction.getInput("stepSize"_c).setDefaultValue(currentStepSize);
		this->kernelAction.getInput("globalSize"_c).setDefaultValue(Range{currentRange,1,1});
		this->kernelAction.run();
		std::cout << "kernel done" << std::endl;

		if(this->currentStepSize == 1){
			return true;
		}else{
			this->filterKernelAction.getInput("positionBuffer"_c).setDefaultValue(*this->positionBuffer1);
			this->filterKernelAction.getInput("stepSize"_c).setDefaultValue(currentStepSize);
			this->filterKernelAction.getInput("globalSize"_c).setDefaultValue(Range{filterRange,1,1});
			this->filterKernelAction.run();

			std::cout << "filter done" << std::endl;

			this->atomicIndexBuffer.copyFromBuffer({0},0,1);
			this->buildBufferAction.getInput("positionBuffer"_c).setDefaultValue(*this->positionBuffer1);
			this->buildBufferAction.getInput("newPositionBuffer"_c).setDefaultValue(*this->positionBuffer2);
			this->buildBufferAction.getInput("atomicIndex"_c).setDefaultValue(this->atomicIndexBuffer);
			this->buildBufferAction.getInput("globalSize"_c).setDefaultValue(Range{filterRange,1,1});
			this->buildBufferAction.getInput("stepSize"_c).setDefaultValue(currentStepSize);
			this->buildBufferAction.run();

			std::cout << "build buffer done" << std::endl;

			std::vector<uint32_t> atomicIndexVector;
			this->atomicIndexBuffer.copyToBuffer(atomicIndexVector);
			uint32_t newRange = atomicIndexVector.front();

			std::swap(this->positionBuffer1,this->positionBuffer2);
			this->currentRange = newRange;
			this->currentStepSize /= 2;
			return false;
		}
	}
};


