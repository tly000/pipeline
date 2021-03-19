#pragma once
#include "../Factory.h"
#include "../Kernel.h"
#include "CL.h"

/*
 * CLFactory.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

struct CLFactory : Factory{
	CLFactory(uint32_t deviceID = -1);

    std::shared_ptr<RawBuffer> createBuffer(std::size_t elemCount, std::size_t elemSize) override;
    std::shared_ptr<RawKernel> createKernel(const std::string& progName,const std::string& kernelName,const std::string& compilerParams);
    std::shared_ptr<RawImage> createImage(std::size_t width, std::size_t height, std::size_t elemSize) override;

	std::string getDeviceName() const;

	static uint32_t getNumberOfDevices();
protected:
	cl::Device device;
	cl::Context context;
	cl::CommandQueue queue;

	static void initOpenCL();

	static bool openCLInitialized;
	static std::vector<cl::Device> devices;
};
