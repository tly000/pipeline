#include "CLFactory.h"

/*
 * CLFactory.cpp
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

void CLFactory::initOpenCL() {
	std::vector<cl::Platform> platforms;
	cl::Platform::get(&platforms);

	for(auto& platform : platforms){
		std::vector<cl::Device> devs;
		platform.getDevices(CL_DEVICE_TYPE_ALL,&devs);
		for(auto& dev : devs){
			if(dev.getInfo<CL_DEVICE_AVAILABLE>()){
				//try to make a context.
				try{
					cl::Context context(dev);
					devices.push_back(dev);
				}catch(...){
					//this device doesnt work.
				}
			}
		}
	}
	openCLInitialized = true;
}

bool CLFactory::openCLInitialized = false;
std::vector<cl::Device> CLFactory::devices{};



