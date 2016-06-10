#pragma once
#include "CalculationActionBase.h"

/*
 * MarianiSilverAction.h
 *
 *  Created on: 08.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct MarianiSilverAction :
	CalculationActionBase<Factory,T,
		KV("positionBuffer",UInt2Buffer<Factory>)>{

	MarianiSilverAction(Factory& f, std::string typeName)
	  :CalculationActionBase<Factory,T,KV("positionBuffer",UInt2Buffer<Factory>)>(f,typeName),
	   factory(f),
	   finalActionGenerator(f),
	   positionBufferGenerator(f),
	   filterBufferGenerator(f){
		this->kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("marianiSilverKernel");

		this->finalActionGenerator.getInput("programName"_c).setDefaultValue("calculation");
		this->finalActionGenerator.getInput("kernelName"_c).setDefaultValue("successiveIterationFill");
		this->definesAction.naturalConnect(this->finalActionGenerator);
		this->finalActionGenerator.naturalConnect(this->finalAction);

		this->delegateInput("imageRange"_c, bufferRangeAction.getInput("imageRange"_c));
		this->delegateInput("iterationImage"_c, finalAction.getInput("iterationImage"_c));

		this->bufferRangeAction.naturalConnect(this->positionBufferGenerator);
		this->bufferRangeAction.naturalConnect(this->filterBufferGenerator);

		this->positionBufferGenerator.template output<0>() >> this->kernelAction.getInput("positionBuffer"_c);
		this->positionBufferGenerator.template output<0>() >> this->finalAction.getInput("positionBuffer"_c);
	}
protected:
	KernelGeneratorAction<Factory,FloatImage<Factory>,UInt2Buffer<Factory>> finalActionGenerator;
	KernelAction<Factory,Input(
		KV("iterationImage",FloatImage<Factory>),
		KV("positionBuffer",UInt2Buffer<Factory>)
	),KernelOutput<>> finalAction;

	Factory& factory;

	BufferGeneratorAction<Factory,Vec<2,uint32_t>> positionBufferGenerator;
	BufferGeneratorAction<Factory,uint8_t> filterBufferGenerator;
	BufferGeneratorAction<Factory,Vec<4,uint32_t>> rectangleBufferGenerator;

	FunctionCallAction<Input(KV("imageRange",Range)),KV("elemCount",uint32_t)> bufferRangeAction{
		[](const Range& r){
			return r.x * r.y;
		}
	};

	int32_t currentSquareSize = -1;
	std::vector<Vec<4,uint32_t>> currentRectangles;
	std::vector<Vec<2,uint32_t>> currentPositionVector;

	void reset(){
		currentSquareSize = -1;
		currentSquares.clear();
		currentPositionVector.clear();
	}

	bool step(){
		Range imageRange = this->getInput("imageRange"_c).getValue();
		uint32_t maxIter = this->getInput("iterations"_c).getValue();

		assertOrThrow(imageRange.x == imageRange.y);

		if(currentSquareSize == -1){
			currentSquareSize = imageRange.x;
			currentRectangles.push_back({0,0,imageRange.x,imageRange.y});
			currentPositionVector.reserve(4 * imageRange.x);
			for(int i = 0; i < imageRange.x; i++){
				currentPositionVector.push_back({i,0});
				if(i != 0){
					currentPositionVector.push_back({0,i});
					currentPositionVector.push_back({i,imageRange.x - 1});
					if(i != imageRange.x - 1){
						currentPositionVector.push_back({imageRange.x - 1,i});
					}
				}
			}
		}

		uint32_t currentRange = this->currentPositionVector.size();

		this->positionBufferGenerator.run();
		auto& positionBuffer = this->positionBufferGenerator.template getOutput<0>().getValue();
		positionBuffer.copyFromBuffer(currentPositionVector,0,currentRange);

		this->kernelAction.getInput("globalSize"_c).setDefaultValue(Range{currentRange,1,1});
		this->kernelAction.run();

		std::vector<uint8_t> filterVector;
		auto& filterBuffer = this->kernelAction.getInput("filterBuffer"_c).getValue();
		filterBuffer.copyToBuffer(filterVector);

		static std::vector<Vec<2,uint32_t>> newPositionVector;
		newPositionVector.clear();
		for(uint32_t i = 0; i < currentRange; i++){
			if(!filterVector.at(i)){
				newPositionVector.push_back(this->currentPositionVector.at(i));
			}
		}

		if(!decreasingPointCount){
			decreasingPointCount = this->currentPositionVector.size() > newPositionVector.size();
		}else if(decreasingPointCount && this->currentPositionVector.size() - newPositionVector.size() < 10){
			this->finalAction.getInput("globalSize"_c).setDefaultValue(Range{uint32_t(this->currentPositionVector.size()),1,1});
			this->finalAction.run();
			return true;
		}

		this->currentPositionVector = newPositionVector;
		this->currentIter += 100;

		return this->currentIter >= maxIter;
	}
};


