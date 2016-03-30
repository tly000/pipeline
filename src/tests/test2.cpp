#include "../Actions/BoxedAction.h"
#include "../Actions/ParameterAction.h"
#include "../Memory/CPU/CPUImage.h"
#include <iostream>

/*
 * test2.cpp
 *
 *  Created on: 30.03.2016
 *      Author: tly
 */

struct BoxFilterAction : StaticPipelineAction<Input(CPUImage<int>,int),Output(CPUImage<int>)>{
	BoxFilterAction(){
		this->getOutput<0>().setValue(CPUImage<int>(1,1));
	}
protected:
	int clip(int i,int h){
		return i < 0 ? 0 : i > h-1 ? h-1 : i;
	}

	virtual void execute(){
		const CPUImage<int>& inputImage = this->getInput<0>().getValue();
		const size_t w = inputImage.getWidth(),
					 h = inputImage.getHeight();

		CPUImage<int>& outputImage = this->getOutput<0>().getValue();
		if(outputImage.getWidth() != w ||
		   outputImage.getHeight() != h){
			outputImage = CPUImage<int>(w,h);
		}

		const int radius = this->getInput<1>().getValue();

		for(size_t x = 0; x < w; x++){
			for(size_t y = 0; y < h; y++){
				int val = 0;
				for(int i = -radius; i <= radius; i++){
					for(int j = -radius; j <= radius; j++){
						val += inputImage.at(clip(x+i,w),clip(y+j,h));
					}
				}
				outputImage.at(x,y) = val / ((2 * radius +1) * (2 * radius +1));
			}
		}
	}
};

int main(){
	CPUImage<int> testImage(100,100);
	for(size_t x = 0; x < 100; x++){
		for(size_t y = 0; y < 100; y++){
			testImage.at(x,y) = std::rand() % 256;
		}
	}

	ParameterAction<CPUImage<int>> imageParam;
	imageParam.setValue(testImage);
	ParameterAction<int> radiusParam;
	radiusParam.setValue(5);

	BoxFilterAction filter;
	imageParam.connectTo(filter,IntPair<0,0>{});
	radiusParam.connectTo(filter,IntPair<0,1>{});

	filter.run();

	auto& outputImage = filter.getOutput<0>().getValue();
}


