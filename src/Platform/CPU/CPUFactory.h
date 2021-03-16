#pragma once
#include "../Factory.h"

#include <map>
/*
 * CPUFactory.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

struct CPUFactory : Factory{
    std::shared_ptr<RawBuffer> createBuffer(std::size_t elemCount, std::size_t elemSize) override;

    std::shared_ptr<RawKernel> createKernel(const std::string& progName,const std::string& kernelName,const std::string& compilerParams) override;

	std::string getDeviceName() const override {
		return "C++ on CPU";
	}
};


