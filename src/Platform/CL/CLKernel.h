#pragma once
#include <utility>

#include "../../Utility/VariadicUtils.h"
#include "../Kernel.h"
#include "CL.h"

/*
 * CLKernel.h
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

struct CLKernel : RawKernel {
    CLKernel(cl::CommandQueue queue, cl::Kernel kernel) : queue(std::move(queue)), kernel(std::move(kernel)) {}

    void run(const Range &globalOffset, const Range &globalSize, const Range &localSize) override {
        cl::NDRange offset(globalOffset.x, globalOffset.y, globalOffset.z);
        cl::NDRange global(globalSize.x, globalSize.y, globalSize.z);
        cl::NDRange local(localSize.x, localSize.y, localSize.z);
        queue.enqueueNDRangeKernel(kernel, offset, global /*,local*/);
        queue.finish();
    }

    void setArg(int i, const void *data, std::size_t size) override {
        kernel.setArg(i, size, data);
    }
    void setArg(int i, const RawBuffer &buffer) override {
        kernel.setArg(i, dynamic_cast<const CLBuffer&>(buffer).getHandle());
    }
protected:
    cl::CommandQueue queue;
    cl::Kernel kernel;
};
