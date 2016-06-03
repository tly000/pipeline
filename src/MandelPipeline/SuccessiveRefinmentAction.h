#pragma once
#include "CalculationActionBase.h"
#include "../Actions/BufferGeneratorAction.h"

/*
 * SuccessiveRefinmentAction.h
 *
 *  Created on: 02.06.2016
 *      Author: tly
 */

template<typename Factory> using UInt2Buffer = typename Factory::template Buffer<Vec<2,uint32_t>>;
template<typename Factory> using UCharBuffer = typename Factory::template Buffer<uint8_t>;

template<typename Factory,typename T> struct SuccessiveRefinementAction :
	CalculationActionBase<Factory,T,KV("positionBuffer",UInt2Buffer<Factory>)>{

	SuccessiveRefinementAction(Factory& f, std::string typeName)
	  :CalculationActionBase<Factory,T,KV("positionBuffer",UInt2Buffer<Factory>)>(f,typeName),
	   factory(f),
	   filterKernelGenerator(f),
	   positionBufferGenerator(f),
	   filterBufferGenerator(f){
		this->kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("successiveRefinementKernel");

		this->filterKernelGenerator.getInput("kernelName"_c).setDefaultValue("successiveRefinementFilter");
		this->filterKernelGenerator.getInput("programName"_c).setDefaultValue("calculation");
		this->filterKernelGenerator.naturalConnect(filterKernelAction);

		this->delegateInput("imageRange"_c, bufferRangeAction.getInput(""))
		this->delegateInput("iterationImage"_c,filterKernelAction.getInput("iterationImage"_c));
	}
protected:
	Factory& factory;

	KernelGeneratorAction<Factory,FloatImage<Factory>,uint32_t,UInt2Buffer<Factory>,UCharBuffer<Factory>> filterKernelGenerator;
	BufferGeneratorAction<Factory,Vec<2,uint32_t>> positionBufferGenerator;
	BufferGeneratorAction<Factory,uint8_t> filterBufferGenerator;

	FunctionCallAction<Input(KV("imageRange",Range)),KV("elemCount",uint32_t)> bufferRangeAction{
		[](const Range& r){
			return r.x * r.y;
		}
	};

	KernelAction<Factory,Input(
		KV("iterationImage",FloatImage<Factory>),
		KV("stepSize",uint32_t),
		KV("positionBuffer",UInt2Buffer<Factory>),
		KV("filterBuffer",UCharBuffer<Factory>)
	),KernelOutput<3>> filterKernelAction;

	UInt2Buffer<Factory> positionBuffer;
	UCharBuffer<Factory> filterBuffer;

	uint32_t currentRange = 0;
	uint32_t currentStepSize = 16;
	std::vector<Vec<2,uint32_t>> currentPositionVector;

	void reset(){
		currentStepSize = 16;
	}

	bool step(){
		Range imageRange = this->getInput("imageRange"_c).getValue();
		if(currentStepSize == 16){
			positionBuffer = factory.template createBuffer<Vec<2,uint32_t>>(imageRange.x * imageRange.y);
			filterBuffer = factory.template createBuffer<uint8_t>(imageRange.x * imageRange.y);
			currentPositionVector.resize(imageRange.x / 16 * imageRange.y / 16);
			int k = 0;
			for(uint32_t i = 0; i < imageRange.x; i+= 2 * currentStepSize){
				for(uint32_t j = 0; j < imageRange.y; j+= 2 * currentStepSize){
					currentPositionVector.at(k++) = {i + currentStepSize,j};
					currentPositionVector.at(k++) = {i ,j + currentStepSize};
					currentPositionVector.at(k++) = {i + currentStepSize,j + currentStepSize};
				}
			}
			for(uint32_t i = 0; i < imageRange.x; i+= 2 * currentStepSize){
				for(uint32_t j = 0; j < imageRange.y; j+= 2 * currentStepSize){
					currentPositionVector.at(k++) = {i,j};
				}
			}
		}
		this->currentRange = this->currentPositionVector.size();
		this->positionBuffer.copyFromBuffer(currentPositionVector,0,currentRange);

		this->kernelAction.getInput("globalSize"_c).setDefaultValue(Range{currentRange,1,1});
		this->kernelAction.getInput("positionBuffer"_c).setDefaultValue(positionBuffer);
		this->kernelAction.run();

		uint32_t filterRange = currentStepSize == 16 ? this->currentRange / 4 : this->currentRange / 3;
		this->filterKernelAction.getInput("positionBuffer"_c).setDefaultValue(positionBuffer);
		this->filterKernelAction.getInput("filterBuffer"_c).setDefaultValue(filterBuffer);
		this->filterKernelAction.getInput("stepSize"_c).setDefaultValue(currentStepSize);
		this->filterKernelAction.getInput("globalSize"_c).setDefaultValue(Range{filterRange,1,1});
		this->filterKernelAction.run();

		std::vector<uint8_t> filterVector;
		this->filterBuffer.copyToBuffer(filterVector);

		std::vector<Vec<2,uint32_t>> newPositionVector;
		for(int i = 0; i < filterRange; i++){
			if(!filterVector.at(i)){
				auto point = this->currentPositionVector.at(3 * i + 2);

				auto p = point - currentStepSize;
				newPositionVector.push_back(point + Vec<2,uint32_t>{currentStepSize / 2, 0});
				newPositionVector.push_back(point + Vec<2,uint32_t>{0, currentStepSize / 2});
				newPositionVector.push_back(point + Vec<2,uint32_t>{currentStepSize / 2, currentStepSize / 2});

				p = point - Vec<2,uint32_t>{currentStepSize,0};
				newPositionVector.push_back(point + Vec<2,uint32_t>{currentStepSize / 2, 0});
				newPositionVector.push_back(point + Vec<2,uint32_t>{0, currentStepSize / 2});
				newPositionVector.push_back(point + Vec<2,uint32_t>{currentStepSize / 2, currentStepSize / 2});

				p = point - Vec<2,uint32_t>{0,currentStepSize};
				newPositionVector.push_back(point + Vec<2,uint32_t>{currentStepSize / 2, 0});
				newPositionVector.push_back(point + Vec<2,uint32_t>{0, currentStepSize / 2});
				newPositionVector.push_back(point + Vec<2,uint32_t>{currentStepSize / 2, currentStepSize / 2});

				p = point;
				newPositionVector.push_back(point + Vec<2,uint32_t>{currentStepSize / 2, 0});
				newPositionVector.push_back(point + Vec<2,uint32_t>{0, currentStepSize / 2});
				newPositionVector.push_back(point + Vec<2,uint32_t>{currentStepSize / 2, currentStepSize / 2});
			}
		}

		this->currentPositionVector = newPositionVector;
		this->currentStepSize /= 2;
		return currentStepSize == 1;
	}
};


