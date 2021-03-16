//
// Created by tly on 14.03.2021.
//

#include "MarianiSilverAction.h"

MarianiSilverAction::MarianiSilverAction(Factory &f)
    :CalculationActionBase(f),
     factory(f),
     positionBufferGenerator(f),
     rectangleBufferGenerator1(f),
     rectangleBufferGenerator2(f),
     filterBufferGenerator(f),
     filterKernelGenerator(f),
     buildBufferActionGenerator(f),
     atomicIndexBuffer(Buffer<std::uint32_t>(f.createBuffer(2, sizeof(std::uint32_t)))){
    this->kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("marianiSilverKernel");

    this->definesAction.naturalConnect(this->filterKernelGenerator);
    this->filterKernelGenerator.getInput(_C("kernelName")).setDefaultValue("marianiSilverFilter");
    this->filterKernelGenerator.getInput(_C("programName")).setDefaultValue("calculation");
    this->filterKernelGenerator.naturalConnect(filterKernelAction);

    this->buildBufferActionGenerator.getInput(_C("programName")).setDefaultValue("calculation");
    this->buildBufferActionGenerator.getInput(_C("kernelName")).setDefaultValue("marianiSilverBuildBuffers");
    this->definesAction.naturalConnect(this->buildBufferActionGenerator);
    this->buildBufferActionGenerator.naturalConnect(this->buildBufferAction);

    this->delegateInput(_C("imageRange"), bufferRangeAction.getInput(_C("imageRange")));
    this->iterationImageGenerator.getOutput<0>() >> filterKernelAction.getInput(_C("iterationImage"));

    this->bufferRangeAction.naturalConnect(this->positionBufferGenerator);
    this->bufferRangeAction.naturalConnect(this->rectangleBufferGenerator1);
    this->bufferRangeAction.naturalConnect(this->rectangleBufferGenerator2);
    this->bufferRangeAction.naturalConnect(this->filterBufferGenerator);

    this->filterBufferGenerator.template output<0>() >> this->filterKernelAction.getInput(_C("filterBuffer"));
    this->filterBufferGenerator.template output<0>() >> this->buildBufferAction.getInput(_C("filterBuffer"));
    this->positionBufferGenerator.template output<0>() >> this->buildBufferAction.getInput(_C("positionBuffer"));
    this->positionBufferGenerator.template output<0>() >> this->kernelAction.getInput(_C("positionBuffer"));
}

bool MarianiSilverAction::step() {
    Range imageRange = this->getInput(_C("imageRange")).getValue();
    if(currentStepSize == -1){
        this->currentStepSize = min(imageRange.x,imageRange.y);
        std::vector<Vec<2,uint32_t>> currentPositionVector;
        currentPositionVector.reserve(2 * (imageRange.x + imageRange.y));
        for(uint32_t i = 0; i < imageRange.x; i ++){
            currentPositionVector.push_back({i,0});
            currentPositionVector.push_back({i,imageRange.y-1});
        }
        for(uint32_t i = 1; i < imageRange.y-1; i ++){
            currentPositionVector.push_back({0,i});
            currentPositionVector.push_back({imageRange.x-1,i});
        }
        this->positionBufferGenerator.run();
        this->rectangleBufferGenerator1.run();
        this->rectangleBufferGenerator2.run();

        this->rectangleCount = 1;
        this->currentRange = currentPositionVector.size();

        this->rectBuffer1 = &this->rectangleBufferGenerator1.template getOutput<0>().getValue();
        this->rectBuffer2 = &this->rectangleBufferGenerator2.template getOutput<0>().getValue();
        auto& positionBuffer = this->positionBufferGenerator.template getOutput<0>().getValue();
        positionBuffer.copyFromBuffer(currentPositionVector.data(), currentPositionVector.data() + currentPositionVector.size());
        Rectangle rectangle{0,0,imageRange.x,imageRange.y};
        this->rectBuffer1->copyFromBuffer(&rectangle, &rectangle + 1);
    }

    if(this->currentStepSize == 2){
        std::vector<Rectangle> remainingRects;
        remainingRects.resize(rectangleCount);
        this->rectBuffer2->copyToBuffer(remainingRects.data(), remainingRects.data() + remainingRects.size());
        std::vector<Vec<2,uint32_t>> positionVector;
        for(const auto& r : remainingRects) {
            for(int i = 1; i < r.w-1; i++){
                for(int j = 1; j < r.h-1; j++){
                    positionVector.push_back({r.x + i, r.y + j});
                }
            }
        }

        auto& positionBuffer = this->positionBufferGenerator.template getOutput<0>().getValue();
        positionBuffer.copyFromBuffer(positionVector.data(), positionVector.data() + positionVector.size());

        this->kernelAction.getInput(_C("globalSize")).setDefaultValue(Range{uint32_t(positionVector.size()),1,1});
        this->kernelAction.run();
        return true;
    }else{
        this->kernelAction.getInput(_C("globalSize")).setDefaultValue(Range{this->currentRange,1,1});
        this->kernelAction.run();

        this->filterKernelAction.getInput(_C("rectangleBuffer")).setDefaultValue(*this->rectBuffer1);
        this->filterKernelAction.getInput(_C("globalSize")).setDefaultValue(Range{this->rectangleCount,1,1});
        this->filterKernelAction.run();

        std::uint32_t data[2]{0,0};
        this->atomicIndexBuffer.copyFromBuffer(std::begin(data), std::end(data));
        this->buildBufferAction.getInput(_C("rectangleBuffer")).setDefaultValue(*this->rectBuffer1);
        this->buildBufferAction.getInput(_C("newRectangleBuffer")).setDefaultValue(*this->rectBuffer2);
        this->buildBufferAction.getInput(_C("atomicIndex")).setDefaultValue(this->atomicIndexBuffer);
        this->buildBufferAction.getInput(_C("globalSize")).setDefaultValue(Range{this->rectangleCount,1,1});
        this->buildBufferAction.run();

        this->atomicIndexBuffer.copyToBuffer(std::begin(data), std::end(data));
        std::swap(this->rectBuffer1,this->rectBuffer2);
        this->currentRange = data[0];
        this->rectangleCount = data[1];
        this->currentStepSize /= 2;
        return this->rectangleCount == 0;
    }
}

void MarianiSilverAction::reset() {
    currentStepSize = -1;
    currentRange = 0;
}
