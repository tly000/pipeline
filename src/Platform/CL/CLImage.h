//
// Created by tly on 16.03.2021.
//

#pragma once
#include "../Image.h"
#include "CLBuffer.h"

struct CLImage : CLBuffer, RawImage {
    CLImage(cl::Context &ctx, cl::CommandQueue queue, cl_mem_flags memFlags, size_t width, size_t height,
            size_t elemSize)
        : CLBuffer(ctx, queue, memFlags, width * height, elemSize), width(width), height(height){}

    std::size_t getWidth() const {
        return width;
    }
    std::size_t getHeight() const {
        return height;
    }
protected:
    std::size_t width;
    std::size_t height;
};