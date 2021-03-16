#pragma once
#include <cstdint>
#include <memory>
#include <vector>

/*
 * Buffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */
struct RawBuffer {
    virtual std::size_t getElementCount() const = 0;
    virtual std::size_t getElementByteSize() const = 0;
    virtual ~RawBuffer() = default;

    virtual void copyToBuffer(void* data, std::size_t size) const = 0;
    virtual void copyFromBuffer(const void *data, std::size_t size) = 0;
};

template<typename T>
struct Buffer {
    Buffer(std::shared_ptr<RawBuffer> rawBuffer) : rawBuffer(rawBuffer) {
        assertOrThrow(rawBuffer->getElementByteSize() == sizeof(T));
    }

    std::size_t getElementCount() const {
        return rawBuffer->getElementCount();
    }
    std::size_t getElementByteSize() const {
        return sizeof(T);
    }

    void copyToBuffer(T *begin, T *end) const {
        rawBuffer->copyToBuffer(begin, (end - begin) * sizeof(T));
    }
    void copyFromBuffer(const T *begin, const T* end) {
        rawBuffer->copyFromBuffer(begin, (end - begin) * sizeof(T));
    }

    std::shared_ptr<RawBuffer> getRawBuffer() const {
        return rawBuffer;
    }
protected:
    std::shared_ptr<RawBuffer> rawBuffer;
};
