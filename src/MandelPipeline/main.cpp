#include "PositionAction.h"
#include "../Actions/ParameterAction.h"
#include "../Platform/CPU/CPUFactory.h"

/*
 * main.cpp
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

int main(){
	ParameterAction<uint32_t> widthParam, heightParam;
	widthParam.setValue(512);
	heightParam.setValue(512);

	PositionAction<CPUFactory,float> posAction;
	posAction.run();
}




