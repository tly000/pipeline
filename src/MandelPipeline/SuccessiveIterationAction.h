#pragma once
#include "CalculationActionBase.h"
#include "../Actions/FunctionCallAction.h"
#include "../Actions/BufferGeneratorAction.h"

/*
 * SuccessiveIterationAction.h
 *
 *  Created on: 06.06.2016
 *      Author: tly
 */

struct SuccessiveIterationAction : CalculationActionBase<
                                       KV("positionBuffer", UInt2Buffer),
                                       KV("filterBuffer", UCharBuffer),
                                       KV("first", uint8_t)> {

    SuccessiveIterationAction();

protected:
    KernelGeneratorAction<FloatImage, UInt2Buffer> finalActionGenerator;
    KernelAction<Input(
                     KV("iterationImage", FloatImage),
                     KV("positionBuffer", UInt2Buffer)),
                 KernelOutput<>>
        finalAction;

    KernelGeneratorAction<UInt2Buffer, UCharBuffer, UInt2Buffer, UIntBuffer> buildBufferActionGenerator;
    KernelAction<Input(
                     KV("positionBuffer", UInt2Buffer),
                     KV("filterBuffer", UCharBuffer),
                     KV("newPositionBuffer", UInt2Buffer),
                     KV("atomicIndex", UIntBuffer)),
                 KernelOutput<>>
        buildBufferAction;

    BufferGeneratorAction<Vec<2, uint32_t>> positionBufferGenerator1;
    BufferGeneratorAction<Vec<2, uint32_t>> positionBufferGenerator2;
    BufferGeneratorAction<uint8_t> filterBufferGenerator;
    BufferGeneratorAction<uint32_t> atomicIndexBufferGenerator;

    UInt2Buffer *positionBuffer1, *positionBuffer2;

    FunctionCallAction<Input(KV("imageRange", Range)), KV("elemCount", uint32_t)> bufferRangeAction{
        [](const Range &r) {
            return r.x * r.y;
        }};

    uint32_t currentIter = 0;
    const uint32_t iterStep = 100;
    bool decreasingPointCount = false;
    uint32_t currentRange = 0;

    void reset();

    bool step();
};
