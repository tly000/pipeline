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
	ParameterAction<
		KV("p1",int),
		KV("p2",int),
		KV("p3",int)> param;
	param.getOutput("p1"_c).setValue(11);
	param.getOutput("p2"_c).setValue(22);
	param.getOutput("p3"_c).setValue(33);

	SumAction sumAction;
	param.output<0,1,2>() >> sumAction.input<0,1,2>();

	sumAction.run();
	std::cout << sumAction.getOutput<0>().getValue() << std::endl;
}



