//
// Created by tly on 14.03.2021.
//
#include "MandelPipeline.h"

MandelPipeline::MandelPipeline(std::shared_ptr<Factory> factory)
    : factory(factory), positionAction(*factory), coloringAction(*factory), reductionAction(*factory),
      creationMap{
          {"normal", [this] { return std::make_unique<NormalCalculationAction>(*this->factory); }},
          {"grid", [this] { return std::make_unique<GridbasedCalculationAction>(*this->factory); }},
          {"successive refinement", [this] { return std::make_unique<SuccessiveRefinementAction>(*this->factory); }},
          {"Mariani-Silver", [this] { return std::make_unique<MarianiSilverAction>(*this->factory); }},
          {"successive iteration", [this] { return std::make_unique<SuccessiveIterationAction>(*this->factory); }},
      },
      methodSelectionAction{[this](const CalculationMethod &method) {
          std::string s = method.getString();
          if (!methodMap.count(s)) { methodMap.emplace(s, creationMap.at(s)()); }
          return methodMap.at(method.getString()).get();
      }} {
    this->addParam(typeParam);
    this->addParam(imageParams);
    this->addParam(multisampleParams);
    this->addParam(positionParams);
    this->addParam(calcParams);
    this->addParam(algoParams);
    this->addParam(colorParams);

    imageParams.naturalConnect(imageRangeGenerator);
    multisampleParams.naturalConnect(imageRangeGenerator);
    imageParams.naturalConnect(reducedImageRangeGenerator);

    //connect positionaction
    positionParams.naturalConnect(convertedPositionParams);
    typeParam.naturalConnect(convertedPositionParams);
    typeParam.naturalConnect(positionAction);
    convertedPositionParams.naturalConnect(positionAction);
    multisampleParams.naturalConnect(positionAction);
    imageRangeGenerator.naturalConnect(positionAction);

    //connect calcAction
    calcParams.naturalConnect(parserAction);
    parserAction.naturalConnect(calcAction);

    calcParams.naturalConnect(convertedCalcParams);
    typeParam.naturalConnect(convertedCalcParams);
    typeParam.naturalConnect(calcAction);
    convertedCalcParams.naturalConnect(calcAction);
    algoParams.naturalConnect(calcAction);
    positionAction.naturalConnect(calcAction);

    imageRangeGenerator.naturalConnect(calcAction);

    calcParams.naturalConnect(methodSelectionAction);
    methodSelectionAction.naturalConnect(calcAction);
    calcAction.getInput(_C("reset calculation")).setDefaultValue(true);

    //connect coloringAction
    typeParam.naturalConnect(coloringAction);
    algoParams.naturalConnect(coloringAction);
    calcParams.naturalConnect(coloringAction);
    calcAction.naturalConnect(coloringAction);
    colorParams.naturalConnect(coloringAction);

    imageRangeGenerator.naturalConnect(coloringAction);

    //connect reductionAction
    typeParam.naturalConnect(reductionAction);
    algoParams.naturalConnect(reductionAction);
    multisampleParams.naturalConnect(reductionAction);
    coloringAction.naturalConnect(reductionAction);

    reducedImageRangeGenerator.naturalConnect(reductionAction);

    //set default values:
    typeParam.setValue(_C("numeric type"), "float");

    imageParams.setValue(_C("width"), 512);
    imageParams.setValue(_C("height"), 512);

    multisampleParams.setValue(_C("enable multisampling"), false);
    multisampleParams.setValue(_C("size"), 2);
    multisampleParams.setValue(_C("pattern"), "UNIFORM_GRID");

    positionParams.setValue(_C("center real"), fromString<HighPrecisionType>("-0.5"));
    positionParams.setValue(_C("center imag"), fromString<HighPrecisionType>("0"));
    positionParams.setValue(_C("scale"), fromString<HighPrecisionType>("2"));
    positionParams.setValue(_C("angle"), 0);

    calcParams.setValue(_C("formula"), "z*z + c");
    calcParams.setValue(_C("enable juliamode"), false);
    calcParams.setValue(_C("julia c real"), fromString<HighPrecisionType>("0"));
    calcParams.setValue(_C("julia c imag"), fromString<HighPrecisionType>("0"));
    calcParams.setValue(_C("calculation method"), "normal");
    calcParams.setValue(_C("visualize steps"), false);

    algoParams.setValue(_C("iterations"), 64);
    algoParams.setValue(_C("bailout"), 16);
    algoParams.setValue(_C("disable bailout"), false);
    algoParams.setValue(_C("cycle detection"), false);
    algoParams.setValue(_C("smooth iteration count"), false);
    algoParams.setValue(_C("leading polynomial exponent"), 2);
    algoParams.setValue(_C("visualize cycle detection"), false);
    algoParams.setValue(_C("statistic function"), "noStats");

    colorParams.setValue(_C("outer gradient"), Gradient{{0.0f, 0.0f, 0.0f}, {0.0f, 0.7f, 1.0f}});
    colorParams.setValue(_C("outer curve"), Curve{CurveSegment{{0, 0}, {1, 1}, 1, 1}});
    colorParams.setValue(_C("outer coloring method"), "iterationGradient");
    colorParams.setValue(_C("inner gradient"), Gradient{{0.0f, 0.0f, 0.0f}});
    colorParams.setValue(_C("inner curve"), Curve{CurveSegment{{0, 0}, {1, 1}, 1, 1}});
    colorParams.setValue(_C("inner coloring method"), "flatColor");
}

void MandelPipeline::run() {
    _logDebug("[info] running pipeline " + demangle(typeid(*this)));
    Timer timer;
    timer.start();
    reductionAction.run();
    auto fullTime = timer.stop();
    _log("[info] full: " << fullTime << " us.");
}
