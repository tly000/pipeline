#pragma once
#include "../Type/Range.h"
#include "Buffer.h"
#include "Image.h"
#include <variant>
#include "../Utility/VariadicUtils.h"

/*
 * Kernel.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

struct RawKernel {
    virtual void setArg(int i, const void* data, std::size_t size) = 0;
    virtual void setArg(int i, const RawBuffer& buffer) = 0;
    virtual void setArg(int i, const RawImage& image) = 0;

    virtual void run(
        const Range &globalOffset,
        const Range &globalSize,
        const Range &localSize) = 0;

    virtual ~RawKernel() = default;
};

template<typename... Inputs> struct Kernel {
    Kernel(std::shared_ptr<RawKernel> rawKernel) : rawKernel(rawKernel) {}

    void run(const Range &globalOffset,
             const Range &globalSize,
             const Range &localSize, Inputs&... inputs) {
        int i = 0;
        variadicForEach(this->forwardArg(i,inputs));
        rawKernel->run(globalOffset, globalSize, localSize);
    }
protected:
    std::shared_ptr<RawKernel> rawKernel;

    // setArg helpers:
    template<typename T> void forwardArg(int& i, const T& data) {
        rawKernel->setArg(i, static_cast<const void*>(&data), sizeof(T));
        i++;
    }

    template<typename... Types> void forwardArg(int& i, const std::variant<Types...>& v) {
        std::visit([&](const auto& v){
            this->forwardArg(i, v);
        }, v);
    }

    template<typename T> void forwardArg(int& i, const Buffer<T>& v) {
        rawKernel->setArg(i, *v.getRawBuffer());
        i++;
    }

    template<typename T> void forwardArg(int& i, const Image<T>& v) {
        rawKernel->setArg(i, *v.getRawImage());
        i++;
    }
};