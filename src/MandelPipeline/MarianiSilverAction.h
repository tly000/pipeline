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

	BufferGeneratorAction<Factory,Vec<2,uint32_t>> positionBufferGenerator;
	BufferGeneratorAction<Factory,Vec<2,uint32_t>> rectangleBufferGenerator1;
	BufferGeneratorAction<Factory,Vec<2,uint32_t>> rectangleBufferGenerator2;
	BufferGeneratorAction<Factory,uint8_t> filterBufferGenerator;

	FunctionCallAction<Input(KV("imageRange",Range)),KV("elemCount",uint32_t)> bufferRangeAction{
		[](const Range& r){
			return r.x * r.y;
		}
	};

	KernelGeneratorAction<Factory,FloatImage<Factory>,UInt2Buffer<Factory>,UCharBuffer<Factory>> filterKernelGenerator;
	KernelAction<Factory,Input(
		KV("iterationImage",FloatImage<Factory>),
		KV("rectangleBuffer",UInt2Buffer<Factory>),
		KV("filterBuffer",UCharBuffer<Factory>)
	),KernelOutput<3>> filterKernelAction;

	KernelGeneratorAction<Factory,UInt2Buffer<Factory>,UCharBuffer<Factory>,UInt2Buffer<Factory>,UIntBuffer<Factory>,uint32_t> buildBufferActionGenerator;
	KernelAction<Factory,Input(
		KV("positionBuffer",UInt2Buffer<Factory>),
		KV("rectangleBuffer",UInt2Buffer<Factory>),
		KV("newRectangleBuffer",UInt2Buffer<Factory>),
		KV("filterBuffer",UCharBuffer<Factory>),
		KV("atomicIndex",UIntBuffer<Factory>),
		KV("stepSize",uint32_t)
	),KernelOutput<>> buildBufferAction;

	UIntBuffer<Factory> atomicIndexBuffer;
	UInt2Buffer<Factory>* rectBuffer1 = nullptr, *rectBuffer2 = nullptr;

	uint32_t currentStepSize = 512;
	uint32_t currentRange = 0;

	void reset(){
		currentStepSize = 512;
		currentRange = 0;
	}

	bool step(){
		Range imageRange = this->getInput("imageRange"_c).getValue();
		uint32_t filterRange = 0;
		if(currentStepSize == 512){
			std::vector<Vec<2,uint32_t>> currentPositionVector;
			currentPositionVector.reserve(4 * imageRange.x);
			for(uint32_t i = 0; i < imageRange.x; i ++){
				currentPositionVector.push_back({i,0});
				currentPositionVector.push_back({i,imageRange.y-1});
				if(i != 0 && i != imageRange.x - 1){
					currentPositionVector.push_back({0,i});
					currentPositionVector.push_back({imageRange.x-1,i});
				}
			}
			currentRange = currentPositionVector.size();

			this->positionBufferGenerator.run();
			this->rectangleBufferGenerator1.run();
			this->rectangleBufferGenerator2.run();

			this->rectBuffer1 = &this->rectangleBufferGenerator1.template getOutput<0>().getValue();
			this->rectBuffer2 = &this->rectangleBufferGenerator2.template getOutput<0>().getValue();
			auto& positionBuffer = this->positionBufferGenerator.template getOutput<0>().getValue();
			positionBuffer.copyFromBuffer(currentPositionVector,0,currentRange);
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





