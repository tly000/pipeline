#pragma once
#include <map>
#include <utility>

#include "../../Utility/VariadicUtils.h"
#include "../Kernel.h"
#include "CL.h"
#include "CLImage.h"

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
        int i = 0;
        for (const auto &[k, setter] : arg_setter) { setter(i); }
        queue.enqueueNDRangeKernel(kernel, offset, global /*,local*/);
        queue.finish();
    }

    void setArg(int i, const void *data, std::size_t size) override {
        arg_setter[i] = [=](int &i) {
            kernel.setArg(i, size, data);
            i++;
        };
    }
    void setArg(int i, const RawBuffer &buffer) override {
        arg_setter[i] = [&](int &i) {
            kernel.setArg(i, dynamic_cast<const CLBuffer &>(buffer).getHandle());
            i++;
            kernel.setArg(i, std::uint32_t(buffer.getElementCount()));
            i++;
        };
    }
    void setArg(int i, const RawImage &image) override {
        arg_setter[i] = [&](int &i) {
            kernel.setArg(i, dynamic_cast<const CLImage &>(image).getHandle());
            i++;
            kernel.setArg(i, std::uint32_t(image.getWidth()));
            i++;
            kernel.setArg(i, std::uint32_t(image.getHeight()));
            i++;
        };
    }

protected:
    std::map<int, std::function<void(int &)>> arg_setter;
    cl::CommandQueue queue;
    cl::Kernel kernel;
};
