#pragma once
#include "../../Utility/Utils.h"
#include "../Buffer.h"
#include <cstring>
#include <memory>
#include <vector>

/*
 * CPUBuffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

struct CPUBuffer : virtual RawBuffer {
    CPUBuffer(std::size_t elemCount, std::size_t elemSize)
        : data(elemCount * elemSize), elemCount(elemCount), elemSize(elemSize) {}

    void copyToBuffer(void *dst, std::size_t n) const override { std::memcpy(dst, this->data.data(), n); }

    void copyFromBuffer(const void *src, std::size_t n) override { std::memcpy(this->data.data(), src, n); }

    size_t getElementCount() const override { return elemCount; }
    size_t getElementByteSize() const override { return elemSize; }

    std::uint8_t* getDataPointer() {
        return this->data.data();
    }

    const std::uint8_t* getDataPointer() const {
        return this->data.data();
    }
protected:
    std::vector<std::uint8_t> data;
    std::size_t elemCount;
    std::size_t elemSize;
};
