//
// Created by tly on 19.03.2021.
//

#pragma once

// warning: type does not match CPUImage type from src!
template<typename T> struct CPUImage {
    std::uint64_t width, height;
    T* data;

    T& at(std::size_t x, std::size_t y) {
        return data[x + y * width];
    }

    const T& at(std::size_t x, std::size_t y) const {
        return data[x + y * width];
    }

    std::uint64_t getWidth() const {
        return width;
    }

    std::uint64_t getHeight() const {
        return height;
    }
};
