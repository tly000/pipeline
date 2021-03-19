//
// Created by tly on 19.03.2021.
//

#pragma once

// warning: type does not match CPUBuffer type from src!
template<typename T> struct CPUBuffer {
    std::uint64_t size;
    T* data;

    T& at(std::size_t x) {
        return data[x];
    }

    const T& at(std::size_t x) const {
        return data[x];
    }

    std::size_t getElemCount() const {
        return size;
    }

    T* getDataPointer() {
        return data;
    }

    const T* getDataPointer() const {
        return data;
    }
};
