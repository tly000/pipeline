#pragma once
//
// Created by tly on 13.03.2021.
//
#include "Image.h"
#include "Buffer.h"
#include "Kernel.h"

struct Factory {
    virtual std::string getDeviceName() const = 0;
    virtual std::shared_ptr<RawBuffer> createBuffer(std::size_t elemCount, std::size_t elemSize) = 0;
    virtual std::shared_ptr<RawKernel> createKernel(const std::string& progName,const std::string& kernelName,const std::string& compilerParams) = 0;
};
