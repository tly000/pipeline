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
	ParameterAction<KV("heyho",int),int,int> param;
	param.setValue<0>(11);
	param.setValue<1>(22);
	param.setValue<2>(33);

	SumAction sumAction;
	param.connectTo(sumAction,IntPair<0,0>{},IntPair<1,1>{},IntPair<2,2>{});

	sumAction.run();
	std::cout << sumAction.getOutput<0>().getValue() << std::endl;
}



