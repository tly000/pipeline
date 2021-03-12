#pragma once
#include "CalculationActionBase.h"

/*
 * GridbasedCalculationAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct GridbasedCalculationAction : CalculationActionBase<Factory,T>{
	using CalculationActionBase<Factory,T>::CalculationActionBase;
protected:
	uint32_t currentPosX = 0, currentPosY = 0;
	const uint32_t gridSize = 50;

	bool step(){
		Range imageRange = this->getInput(_C("imageRange")).getValue();
		if(currentPosY < imageRange.y){
			uint32_t nextPixelsX = std::min(gridSize,imageRange.x - currentPosX);
			uint32_t nextPixelsY = std::min(gridSize,imageRange.y - currentPosY);

			this->kernelAction.getInput(_C("globalOffset")).setDefaultValue(Range{currentPosX,currentPosY,0});
			this->kernelAction.getInput(_C("globalSize")).setDefaultValue(Range{nextPixelsX,nextPixelsY,1});
			this->kernelAction.run();

			currentPosX += gridSize;
			if(currentPosX >= imageRange.x){
				currentPosX = 0;
				currentPosY += gridSize;
			}
			return false;
		}else{
			return true;
		}
	}

	void reset(){
		currentPosX = 0;
		currentPosY = 0;
	}
};

