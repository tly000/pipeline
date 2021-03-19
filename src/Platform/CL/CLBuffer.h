#pragma once
#include "../Buffer.h"
#include "CL.h"
#include <stdexcept>

/*
 * CLBuffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

struct CLBuffer : virtual RawBuffer {
    CLBuffer(cl::Context &ctx, cl::CommandQueue queue, cl_mem_flags memFlags, size_t elemCount, size_t elemSize)
        : queue(queue),
          bufferHandle(ctx, memFlags, elemSize * elemCount),
          elemCount(elemCount),
          elemSize(elemSize){}

    cl::Buffer getHandle() const {
        return bufferHandle;
    }

    void copyToBuffer(void *data, std::size_t size) const {
        queue.enqueueReadBuffer(bufferHandle, true, 0, size, data);
        queue.finish();
    }

    void copyFromBuffer(const void *data, std::size_t size) {
        queue.enqueueWriteBuffer(bufferHandle, true, 0, size, data);
        queue.finish();
    }

    size_t getElementCount() const override { return elemCount; }
    size_t getElementByteSize() const override { return elemSize; }
protected:
    cl::CommandQueue queue;
    cl::Buffer bufferHandle;
    std::size_t elemCount;
    std::size_t elemSize;
};
