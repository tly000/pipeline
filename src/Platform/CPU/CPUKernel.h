#pragma once
#include "../../Utility/DynamicLibrary.h"
#include "../Kernel.h"

/*
 * CPUKernel.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

struct CPUKernel : RawKernel {
    constexpr static std::size_t MaxNumArgs = 64;
    using KernelFunc = void (*)(const Range &globalID, const Range &localID, ...);

    CPUKernel(DynamicLibrary library, KernelFunc kernelFunc) : library(library), kernelFunc(kernelFunc) {
        args.resize(MaxNumArgs); // fill rest of args with nullptr
    }

    void run(const Range &globalOffset, const Range &globalSize, const Range &localSize) {
        runImpl(std::make_index_sequence<MaxNumArgs>(), globalOffset, globalSize, localSize);
        raw_args_storage.clear();
    }

    template<size_t... I> void runImpl(std::index_sequence<I...>, const Range &globalOffset, const Range &globalSize, const Range &localSize) {
        #pragma omp parallel for collapse(3)
        for(auto i = globalOffset.x; i < globalOffset.x + globalSize.x; i++){
            for(auto j = globalOffset.y; j < globalOffset.y + globalSize.y; j++){
                for(auto k = globalOffset.z; k < globalOffset.z + globalSize.z; k++){
                    kernelFunc(Range{i,j,k},Range{0,0,0},args[I]...);
                }
            }
        }
    }

    void setArg(int i, const void *data, std::size_t size) override {
        args[i] = data;
    }
    void setArg(int i, const RawBuffer &buffer) override {
        // match CPUBuffer<T> type used in kernel
        auto& buff = raw_args_storage.emplace_back(2);
        buff[0] = buffer.getElementCount();
        buff[1] = reinterpret_cast<std::uint64_t>(dynamic_cast<const CPUBuffer &>(buffer).getDataPointer());
        setArg(i, buff.data(), 0);
    }
    void setArg(int i, const RawImage &image) override {
        // match CPUImage<T> type used in kernel
        auto& buff = raw_args_storage.emplace_back(2);
        buff[0] = image.getWidth();
        buff[1] = image.getHeight();
        buff[2] = reinterpret_cast<std::uint64_t>(dynamic_cast<const CPUImage &>(image).getDataPointer());
        setArg(i, buff.data(), 0);
    }

protected:
    std::list<std::vector<std::uint64_t>> raw_args_storage;
    std::vector<const void *> args;
    DynamicLibrary library;
    KernelFunc kernelFunc;
};
