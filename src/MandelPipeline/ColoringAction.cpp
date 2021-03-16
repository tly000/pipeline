//
// Created by tly on 14.03.2021.
//
#include "ColoringAction.h"

ColoringAction::ColoringAction(Factory &factory)
    :factory(factory),
     gradientBufferGenerator1(factory),
     gradientBufferGenerator2(factory),
     curveBufferGenerator1(factory),
     curveBufferGenerator2(factory),
     kernelGeneratorAction(factory),
     coloredImageGenerator(factory){
    this->delegateInput(_C("outer gradient"),this->gradientBufferGenerator1.getInput(_C("data")));
    this->delegateInput(_C("inner gradient"),this->gradientBufferGenerator2.getInput(_C("data")));

    this->delegateInput(_C("outer curve"),this->curveBufferGenerator1.getInput(_C("data")));
    this->delegateInput(_C("inner curve"),this->curveBufferGenerator2.getInput(_C("data")));

    this->delegateInput(_C("outer coloring method"),this->definesAction.getInput(_C("OUTSIDE_COLORING_KERNEL")));
    this->delegateInput(_C("inner coloring method"),this->definesAction.getInput(_C("INSIDE_COLORING_KERNEL")));
    this->delegateInput(_C("bailout"),this->definesAction.getInput(_C("BAILOUT")));
    this->delegateInput(_C("enable juliamode"),this->definesAction.getInput(_C("JULIAMODE")));
    this->delegateInput(_C("iterations"),this->definesAction.getInput(_C("MAXITER")));
    this->delegateInput(_C("numeric type"),this->definesAction.getInput(_C("Type")));
    this->definesAction.naturalConnect(kernelGeneratorAction);

    this->delegateInput(_C("imageRange"),this->coloredImageGenerator.getInput(_C("imageRange")));
    kernelGeneratorAction.getInput(_C("programName")).setDefaultValue("coloring");
    kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("coloringKernel");

    kernelGeneratorAction.naturalConnect(kernelAction);

    this->delegateInput(_C("iterationImage"),kernelAction.getInput(_C("iterationImage")));
    this->delegateInput(_C("processedPositionImage"),kernelAction.getInput(_C("processedPositionImage")));
    this->delegateInput(_C("statsImage"),kernelAction.getInput(_C("statsImage")));
    this->coloredImageGenerator.getOutput<0>() >> kernelAction.getInput(_C("coloredImage"));
    this->delegateInput(_C("imageRange"), kernelAction.getInput(_C("globalSize")));
    this->gradientBufferGenerator1.template output<0>() >> this->kernelAction.getInput(_C("gradientA"));
    this->gradientBufferGenerator2.template output<0>() >> this->kernelAction.getInput(_C("gradientB"));
    this->curveBufferGenerator1.template output<0>() >> this->kernelAction.getInput(_C("curveA"));
    this->curveBufferGenerator2.template output<0>() >> this->kernelAction.getInput(_C("curveB"));

    this->delegateOutput(_C("coloredImage"), kernelAction.getOutput(_C("coloredImage")));
}

void ColoringAction::executeImpl() {
    kernelAction.run();
}
