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
struct RawImage : virtual RawBuffer {
    virtual std::size_t getWidth() const = 0;
    virtual std::size_t getHeight() const = 0;
};

template<typename T>
struct Image {
    Image(std::shared_ptr<RawImage> rawImage)
        : rawImage(rawImage) {
        assertOrThrow(rawImage->getElementByteSize() == sizeof(T));
    }

    std::size_t getWidth() const {
        return rawImage->getWidth();
    }
    std::size_t getHeight() const {
        return rawImage->getHeight();
    }

    std::shared_ptr<RawImage> getRawImage() const {
        return rawImage;
    }
protected:
    std::shared_ptr<RawImage> rawImage;
};
