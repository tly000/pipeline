//
// Created by tly on 16.03.2021.
//

#include "CPUBuffer.h"
#include "../Image.h"

struct CPUImage : RawImage, CPUBuffer {
    CPUImage(std::size_t width, std::size_t height, std::size_t elemCount)
        : CPUBuffer(width * height, elemCount), width(width), height(height) {}

    size_t getWidth() const override { return width; }
    size_t getHeight() const override { return height; }
protected:
    std::size_t width;
    std::size_t height;
};