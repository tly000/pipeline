#pragma once
#include "CalculationActionBase.h"

/*
 * GridbasedCalculationAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

struct GridbasedCalculationAction : CalculationActionBase<>{
	using CalculationActionBase<>::CalculationActionBase;
protected:
	uint32_t currentPosX = 0, currentPosY = 0;
	const uint32_t gridSize = 50;

	bool step();

	void reset();
};

