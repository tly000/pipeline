//
// Created by tly on 14.03.2021.
//

#include "SuccessiveRefinementAction.h"

SuccessiveRefinementAction::SuccessiveRefinementAction() {
    this->kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("successiveRefinementKernel");

    this->definesAction.naturalConnect(this->filterKernelGenerator);
    this->filterKernelGenerator.getInput(_C("kernelName")).setDefaultValue("successiveRefinementFilter");
    this->filterKernelGenerator.getInput(_C("programName")).setDefaultValue("calculation");
    this->filterKernelGenerator.naturalConnect(filterKernelAction);

    this->buildBufferActionGenerator.getInput(_C("programName")).setDefaultValue("calculation");
    this->buildBufferActionGenerator.getInput(_C("kernelName")).setDefaultValue("successiveRefinementBuildPositionBuffer");
    this->definesAction.naturalConnect(this->buildBufferActionGenerator);
    this->buildBufferActionGenerator.naturalConnect(this->buildBufferAction);

    this->delegateInput(_C("imageRange"), bufferRangeAction.getInput(_C("imageRange")));
    this->iterationImageGenerator.getOutput<0>() >> filterKernelAction.getInput(_C("iterationImage"));
    this->processedPositionImageGenerator.getOutput<0>() >> filterKernelAction.getInput(_C("processedPositionImage"));

    this->bufferRangeAction.naturalConnect(this->positionBufferGenerator1);
    this->bufferRangeAction.naturalConnect(this->positionBufferGenerator2);
    this->bufferRangeAction.naturalConnect(this->filterBufferGenerator);

    this->filterBufferGenerator.template output<0>() >> this->filterKernelAction.getInput(_C("filterBuffer"));
    this->filterBufferGenerator.template output<0>() >> this->buildBufferAction.getInput(_C("filterBuffer"));

    this->delegateInput(_C("platform"),this->positionBufferGenerator1.getInput(_C("platform")));
    this->delegateInput(_C("platform"),this->positionBufferGenerator2.getInput(_C("platform")));
    this->delegateInput(_C("platform"),this->buildBufferActionGenerator.getInput(_C("platform")));
    this->delegateInput(_C("platform"),this->filterKernelGenerator.getInput(_C("platform")));
    this->delegateInput(_C("platform"),this->atomicIndexBufferGenerator.getInput(_C("platform")));
}

void SuccessiveRefinementAction::reset() {
    currentStepSize = 16;
    currentRange = 0;
}

bool SuccessiveRefinementAction::step() {
    Range imageRange = this->getInput(_C("imageRange")).getValue();
    uint32_t filterRange = 0;
    if(currentStepSize == 16){
        std::vector<Vec<2,uint32_t>> currentPositionVector;
        currentPositionVector.reserve(imageRange.x / 16 * imageRange.y/16);
        for(uint32_t i = 0; i < imageRange.x - 32; i += 32){
            for(uint32_t j = 0; j < imageRange.y - 32; j += 32){
                currentPositionVector.push_back({i + 16,j});
                currentPositionVector.push_back({i ,j + 16});
                currentPositionVector.push_back({i + 16,j + 16});
            }
        }
        filterRange = currentPositionVector.size() / 3;
        for(uint32_t i = 0; i < imageRange.x - 32; i+= 32){
            for(uint32_t j = 0; j < imageRange.y - 32; j+= 32){
                currentPositionVector.push_back({i,j});
            }
        }

        auto restStartPoint = currentPositionVector.back();
        for(uint32_t i = restStartPoint[0] + 32; i < imageRange.x; i++){
            for(uint32_t j = 0; j < restStartPoint[1] + 32; j++){
                currentPositionVector.push_back({i,j});
            }
        }
        for(uint32_t i = 0; i < imageRange.x; i++){
            for(uint32_t j = restStartPoint[1] + 32; j < imageRange.y; j++){
                currentPositionVector.push_back({i,j});
            }
        }

        currentRange = currentPositionVector.size();
        this->positionBufferGenerator1.run();
        this->positionBufferGenerator2.run();
        this->atomicIndexBufferGenerator.run();
        this->positionBuffer1 = &this->positionBufferGenerator1.template getOutput<0>().getValue();
        this->positionBuffer2 = &this->positionBufferGenerator2.template getOutput<0>().getValue();
        this->positionBuffer1->copyFromBuffer(currentPositionVector.data(), currentPositionVector.data() + currentPositionVector.size());
    }else{
        filterRange = currentRange / 3;
    }

    this->kernelAction.getInput(_C("positionBuffer")).setDefaultValue(*this->positionBuffer1);
    this->kernelAction.getInput(_C("stepSize")).setDefaultValue(currentStepSize);
    this->kernelAction.getInput(_C("globalSize")).setDefaultValue(Range{currentRange,1,1});
    this->kernelAction.run();

    if(this->currentStepSize == 1){
        return true;
    }else{
        this->filterKernelAction.getInput(_C("positionBuffer")).setDefaultValue(*this->positionBuffer1);
        this->filterKernelAction.getInput(_C("stepSize")).setDefaultValue(currentStepSize);
        this->filterKernelAction.getInput(_C("globalSize")).setDefaultValue(Range{filterRange,1,1});
        this->filterKernelAction.run();

        auto& atomicIndexBuffer = this->atomicIndexBufferGenerator.getOutput<0>().getValue();
        uint32_t size = 0;
        atomicIndexBuffer.copyFromBuffer(&size, &size + 1);
        this->buildBufferAction.getInput(_C("positionBuffer")).setDefaultValue(*this->positionBuffer1);
        this->buildBufferAction.getInput(_C("newPositionBuffer")).setDefaultValue(*this->positionBuffer2);
        this->buildBufferAction.getInput(_C("atomicIndex")).setDefaultValue(atomicIndexBuffer);
        this->buildBufferAction.getInput(_C("globalSize")).setDefaultValue(Range{filterRange,1,1});
        this->buildBufferAction.getInput(_C("stepSize")).setDefaultValue(currentStepSize);
        this->buildBufferAction.run();

        atomicIndexBuffer.copyToBuffer(&size, &size + 1);

        std::swap(this->positionBuffer1,this->positionBuffer2);
        this->currentRange = size;
        this->currentStepSize /= 2;
        return this->currentRange == 0;
    }
}
