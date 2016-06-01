#pragma once
#include "CalculationAction.h"

/*
 * GridbasedCalculationAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct GridbasedCalculationAction : CalculationAction<Factory,T>{
	using CalculationAction<Factory,T>::CalculationAction;
protected:
	void executeImpl(){
		const uint32_t gridSize = 50;
		Range imageRange = this->getInput("imageRange"_c).getValue();

		Timer t;
		t.start();
		for(uint32_t i = 0; i < imageRange.x; i += gridSize){
			uint32_t remainingPixelsX = std::min(gridSize,imageRange.x - i);
			for(uint32_t j = 0; j < imageRange.y; j += gridSize){
				uint32_t remainingPixelsY = std::min(gridSize,imageRange.y - j);
				this->kernelAction.getInput("globalOffset"_c).setDefaultValue(Range{i,j,0});
				this->kernelAction.getInput("globalSize"_c).setDefaultValue(Range{remainingPixelsX,remainingPixelsY,1});
				this->kernelAction.run();
			}
		}
		uint64_t time = t.stop();
		_log("[info] gridbased calculation: " << time << " us.");
	}
};

