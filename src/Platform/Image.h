#pragma once
#include "../Utility/Utils.h"
#include "Buffer.h"
#include <cstddef>

/*
 * Image.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T>
struct Image {
    Image(std::size_t width, std::size_t height, std::shared_ptr<RawBuffer> rawImage)
        : width(width),
          height(height),
          rawImage(rawImage) {
        assertOrThrow(rawImage->getElementByteSize() == sizeof(T));
        assertOrThrow(rawImage->getElementCount() == width * height);
    }

    std::size_t getWidth() const {
        return width;
    }
    std::size_t getHeight() const {
        return height;
    }

    std::shared_ptr<RawBuffer> getRawImage() const {
        return rawImage;
    }
protected:
    std::shared_ptr<RawBuffer> rawImage;
    std::size_t height;
    std::size_t width;
};
