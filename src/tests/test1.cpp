/*
 * main.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

#include <iostream>
#include "../Pipeline/StaticPipelineAction.h"
#include "../Actions/ParameterAction.h"

struct SumAction : StaticPipelineAction<Input(int,int,int),Output(int)>{
protected:
	void execute(){
		int val = this->getInput<0>().getValue()
				+ this->getInput<1>().getValue()
				+ this->getInput<2>().getValue();
		this->getOutput<0>().setValue(val);
	}
};

void test1(){
	ParameterAction<int> param1,param2,param3;
	param1.setValue(11);
	param2.setValue(22);
	param3.setValue(33);

	SumAction sumAction;
	param1.connectTo(sumAction,IntPair<0,0>{});
	param2.connectTo(sumAction,IntPair<0,1>{});
	param3.connectTo(sumAction,IntPair<0,2>{});

	sumAction.run();
	std::cout << sumAction.getOutput<0>().getValue() << std::endl;
}


