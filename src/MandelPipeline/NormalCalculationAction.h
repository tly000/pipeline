#pragma once
#include "CalculationActionBase.h"

/*
 * NormalCalculation.h
 *
 *  Created on: 02.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct NormalCalculationAction : CalculationActionBase<Factory,T>{
	using CalculationActionBase<Factory,T>::CalculationActionBase;
protected:
	bool step(){
		this->kernelAction.run();
		return true;
	}
	void reset(){}
};


