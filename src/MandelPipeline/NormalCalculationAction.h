#pragma once
#include "CalculationActionBase.h"

/*
 * NormalCalculation.h
 *
 *  Created on: 02.06.2016
 *      Author: tly
 */

struct NormalCalculationAction : CalculationActionBase<>{
	using CalculationActionBase<>::CalculationActionBase;
protected:
	bool step(){
		this->kernelAction.run();
		return true;
	}
	void reset(){}
};


