//
// Created by tly on 14.03.2021.
//

#include "ReductionAction.h"

ReductionAction::ReductionAction() {
    this->delegateInput(_C("enable multisampling"), definesAction.getInput(_C("MULTISAMPLING_ENABLED")));
    this->delegateInput(_C("size"),definesAction.getInput(_C("MULTISAMPLING_SIZE")));
    this->delegateInput(_C("numeric type"),definesAction.getInput(_C("Type")));
    this->delegateInput(_C("imageRange"),reducedImageGenerator.getInput(_C("imageRange")));

    this->delegateInput(_C("platform"),this->reducedImageGenerator.getInput(_C("platform")));
    this->delegateInput(_C("platform"),this->kernelGeneratorAction.getInput(_C("platform")));

    definesAction.naturalConnect(kernelGeneratorAction);
    kernelGeneratorAction.getInput(_C("programName")).setDefaultValue("reduction");
    kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("reductionKernel");

    kernelGeneratorAction.naturalConnect(kernelAction);

    this->delegateInput(_C("coloredImage"), kernelAction.getInput(_C("coloredImage")));
    reducedImageGenerator.getOutput<0>() >> kernelAction.getInput(_C("reducedImage"));
    this->delegateInput(_C("imageRange"), kernelAction.getInput(_C("globalSize")));

    this->delegateOutput(_C("reducedImage"),kernelAction.getOutput(_C("reducedImage")));
}

void ReductionAction::executeImpl() {
    kernelAction.run();
}
