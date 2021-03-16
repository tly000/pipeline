//
// Created by tly on 14.03.2021.
//

#include "SuccessiveIterationAction.h"

bool SuccessiveIterationAction::step() {
    Range imageRange = this->getInput(_C("imageRange")).getValue();
    uint32_t maxIter = this->getInput(_C("iterations")).getValue();

    do {
        if (currentIter == 0) {
            std::vector<Vec<2, uint32_t>> currentPositionVector;
            currentPositionVector.reserve(imageRange.x * imageRange.y);
            for (uint32_t i = 0; i < imageRange.x; i++) {
                for (uint32_t j = 0; j < imageRange.y; j++) {
                    currentPositionVector.push_back({i, j});
                }
            }

            currentRange = currentPositionVector.size();

            this->positionBufferGenerator1.run();
            this->positionBufferGenerator2.run();
            this->positionBuffer1 = &this->positionBufferGenerator1.template getOutput<0>().getValue();
            this->positionBuffer2 = &this->positionBufferGenerator2.template getOutput<0>().getValue();
            this->positionBuffer1->copyFromBuffer(currentPositionVector.data(), currentPositionVector.data() + currentPositionVector.size());
        }

        this->kernelAction.getInput(_C("positionBuffer")).setDefaultValue(*this->positionBuffer1);
        this->kernelAction.getInput(_C("first")).setDefaultValue(currentIter == 0);
        this->kernelAction.getInput(_C("globalSize")).setDefaultValue(Range{currentRange, 1, 1});
        this->kernelAction.run();

        std::uint32_t size = 0;
        this->atomicIndexBuffer.copyFromBuffer(&size, &size + 1);
        this->buildBufferAction.getInput(_C("positionBuffer")).setDefaultValue(*this->positionBuffer1);
        this->buildBufferAction.getInput(_C("newPositionBuffer")).setDefaultValue(*this->positionBuffer2);
        this->buildBufferAction.getInput(_C("atomicIndex")).setDefaultValue(this->atomicIndexBuffer);
        this->buildBufferAction.getInput(_C("globalSize")).setDefaultValue(Range{currentRange, 1, 1});
        this->buildBufferAction.run();

        this->atomicIndexBuffer.copyToBuffer(&size, &size + 1);

        if (!decreasingPointCount) {
            decreasingPointCount = currentRange > size;
        }
        if (size == 0) {
            return true;
        } else if (decreasingPointCount && currentRange - size < 10) {
            this->finalAction.getInput(_C("positionBuffer")).setDefaultValue(*this->positionBuffer1);
            this->finalAction.getInput(_C("globalSize")).setDefaultValue(Range{size, 1, 1});
            this->finalAction.run();
            return true;
        }

        std::swap(this->positionBuffer1, this->positionBuffer2);
        this->currentRange = size;
        this->currentIter += 100;
    } while (!decreasingPointCount);

    return this->currentIter >= maxIter;
}

void SuccessiveIterationAction::reset() {
    currentIter = 0;
    decreasingPointCount = false;
}

SuccessiveIterationAction::SuccessiveIterationAction(Factory &f)
    : CalculationActionBase<KV("positionBuffer", UInt2Buffer), KV("filterBuffer", UCharBuffer), KV("first", uint8_t)>(f),
      factory(f),
      finalActionGenerator(f),
      buildBufferActionGenerator(f),
      positionBufferGenerator1(f),
      positionBufferGenerator2(f),
      filterBufferGenerator(f),
      atomicIndexBuffer(Buffer<std::uint32_t>(f.createBuffer(1, sizeof(std::uint32_t)))) {
    this->kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("successiveIterationKernel");

    this->finalActionGenerator.getInput(_C("programName")).setDefaultValue("calculation");
    this->finalActionGenerator.getInput(_C("kernelName")).setDefaultValue("successiveIterationFill");
    this->definesAction.naturalConnect(this->finalActionGenerator);
    this->finalActionGenerator.naturalConnect(this->finalAction);

    this->buildBufferActionGenerator.getInput(_C("programName")).setDefaultValue("calculation");
    this->buildBufferActionGenerator.getInput(_C("kernelName")).setDefaultValue("successiveIterationBuildPositionBuffer");
    this->definesAction.naturalConnect(this->buildBufferActionGenerator);
    this->buildBufferActionGenerator.naturalConnect(this->buildBufferAction);

    this->delegateInput(_C("imageRange"), bufferRangeAction.getInput(_C("imageRange")));
    this->iterationImageGenerator.getOutput<0>() >> finalAction.getInput(_C("iterationImage"));

    this->bufferRangeAction.naturalConnect(this->positionBufferGenerator1);
    this->bufferRangeAction.naturalConnect(this->positionBufferGenerator2);
    this->bufferRangeAction.naturalConnect(this->filterBufferGenerator);

    this->filterBufferGenerator.template output<0>() >> this->kernelAction.getInput(_C("filterBuffer"));
    this->filterBufferGenerator.template output<0>() >> this->buildBufferAction.getInput(_C("filterBuffer"));
}
