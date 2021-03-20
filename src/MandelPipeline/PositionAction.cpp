//
// Created by tly on 14.03.2021.
//

#include "PositionAction.h"
#include "VariantUtils.h"

PositionAction::PositionAction()
    : positionImageGenerator([&](const std::shared_ptr<Factory>& factory, const NumericType &type, const Range &imageRange) {
          return make_variant_complex_image(*factory, type, imageRange);
      }),
      rotationAction([](const VariantNumericType &scale, float angle) {
          angle *= 3.14159 / 180;
          return std::visit(
              [&]<typename T>(T scale) -> VariantVec2 {
                  return Vec<2, T>{
                      tmul(scale, fromFloatToType<T>(cosf(angle))),
                      tmul(scale, fromFloatToType<T>(sinf(angle))),
                  };
              },
              scale);
      }) {
    this->delegateInput(_C("enable multisampling"), definesAction.getInput(_C("MULTISAMPLING_ENABLED")));
    this->delegateInput(_C("pattern"), definesAction.getInput(_C("MULTISAMPLING_PATTERN")));
    this->delegateInput(_C("numeric type"), definesAction.getInput(_C("Type")));
    this->delegateInput(_C("numeric type"), positionImageGenerator.getInput(_C("numeric type")));
    this->delegateInput(_C("imageRange"), positionImageGenerator.getInput(_C("imageRange")));

    this->delegateInput(_C("platform"),this->positionImageGenerator.getInput(_C("platform")));
    this->delegateInput(_C("platform"),this->kernelGeneratorAction.getInput(_C("platform")));

    definesAction.naturalConnect(kernelGeneratorAction);
    kernelGeneratorAction.getInput(_C("programName")).setDefaultValue("position");
    kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("positionKernel");

    this->delegateInput(_C("scale"), rotationAction.getInput(_C("scale")));
    this->delegateInput(_C("angle"), rotationAction.getInput(_C("angle")));
    rotationAction.naturalConnect(kernelAction);

    kernelGeneratorAction.naturalConnect(kernelAction);
    this->positionImageGenerator.getOutput<0>() >> kernelAction.getInput(_C("positionImage"));
    this->delegateInput(_C("center real"), kernelAction.getInput(_C("center real")));
    this->delegateInput(_C("center imag"), kernelAction.getInput(_C("center imag")));
    this->delegateInput(_C("imageRange"), kernelAction.getInput(_C("globalSize")));

    this->delegateOutput(_C("positionImage"), kernelAction.getOutput(_C("positionImage")));
}

void PositionAction::executeImpl() { kernelAction.run(); }
