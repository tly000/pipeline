#pragma once
#include "../Actions/BoxedAction.h"
#include "../Actions/BufferGeneratorAction.h"
#include "../Actions/FunctionCallAction.h"
#include "../Actions/ImageGeneratorAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelDefinesAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Platform/Factory.h"
#include "../Type/Vec.h"
#include "Enums.h"
#include "Typedefs.h"
#include "VariantUtils.h"

/*
 * CalculationAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

template<typename... AdditionalKernelParams> struct CalculationActionBase :
	BoxedAction<Input(
        KV("numeric type", NumericType),
		KV("visualize steps",bool),
		KV("reset calculation",bool),
		KV("processed formula",std::string),
		KV("enable juliamode",bool),
		KV("julia c real",VariantNumericType),
		KV("julia c imag",VariantNumericType),
		KV("iterations",uint32_t),
		KV("bailout",float),
		KV("disable bailout",bool),
		KV("cycle detection",bool),
		KV("visualize cycle detection",bool),
		KV("smooth iteration count",bool),
		KV("leading polynomial exponent",float),
		KV("statistic function",std::string),
		KV("positionImage",VariantComplexImage),
		KV("imageRange",Range)
	),Output(
		KV("iterationImage",FloatImage),
		KV("processedPositionImage",VariantComplexImage),
		KV("statsImage",Float4Image),
		KV("time",uint64_t),
		KV("done",bool)
	)>{
		CalculationActionBase(Factory& factory)
	  : iterationImageGenerator(factory),
        statsImageGenerator(factory),
        processedPositionImageGenerator([&](const NumericType& type, const Range& imageRange) {
            return make_variant_complex_image(factory, type, imageRange);
        }),
        kernelGeneratorAction(factory) {
		this->delegateInput(_C("processed formula"), definesAction.getInput(_C("FORMULA")));
		this->delegateInput(_C("enable juliamode"), definesAction.getInput(_C("JULIAMODE")));
		this->delegateInput(_C("iterations"), definesAction.getInput(_C("MAXITER")));
		this->delegateInput(_C("bailout"), definesAction.getInput(_C("BAILOUT")));
		this->delegateInput(_C("disable bailout"), definesAction.getInput(_C("DISABLE_BAILOUT")));
		this->delegateInput(_C("cycle detection"), definesAction.getInput(_C("CYCLE_DETECTION")));
		this->delegateInput(_C("visualize cycle detection"), definesAction.getInput(_C("CYCLE_DETECTION_VISUALIZE")));
		this->delegateInput(_C("smooth iteration count"), definesAction.getInput(_C("SMOOTH_MODE")));
		this->delegateInput(_C("leading polynomial exponent"), definesAction.getInput(_C("SMOOTH_EXP")));
		this->delegateInput(_C("statistic function"), definesAction.getInput(_C("STAT_FUNCTION")));
        this->delegateInput(_C("numeric type"), definesAction.getInput(_C("Type")));
        this->delegateInput(_C("numeric type"), processedPositionImageGenerator.getInput(_C("numeric type")));
        this->delegateInput(_C("imageRange"), iterationImageGenerator.getInput(_C("imageRange")));
        this->delegateInput(_C("imageRange"), statsImageGenerator.getInput(_C("imageRange")));
        this->delegateInput(_C("imageRange"), processedPositionImageGenerator.getInput(_C("imageRange")));

		definesAction.naturalConnect(kernelGeneratorAction);
		kernelGeneratorAction.getInput(_C("programName")).setDefaultValue("calculation");
		kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("mandelbrotKernel");

		kernelGeneratorAction.naturalConnect(kernelAction);

		this->delegateInput(_C("positionImage"),kernelAction.getInput(_C("positionImage")));
		this->delegateInput(_C("julia c real"), kernelAction.getInput(_C("julia c real")));
		this->delegateInput(_C("julia c imag"), kernelAction.getInput(_C("julia c imag")));
		this->delegateInput(_C("imageRange"), kernelAction.getInput(_C("globalSize")));

        iterationImageGenerator.template getOutput<0>() >> kernelAction.getInput(_C("iterationImage"));
        statsImageGenerator.template getOutput<0>() >> kernelAction.getInput(_C("statsImage"));
        processedPositionImageGenerator.template getOutput<0>() >> kernelAction.getInput(_C("processedPositionImage"));

        this->delegateOutput(_C("iterationImage"), kernelAction.getOutput(_C("iterationImage")));
		this->delegateOutput(_C("processedPositionImage"), kernelAction.getOutput(_C("processedPositionImage")));
		this->delegateOutput(_C("statsImage"), kernelAction.getOutput(_C("statsImage")));
	}

    ImageGeneratorAction<float> iterationImageGenerator;
    ImageGeneratorAction<Vec<4,float>> statsImageGenerator;
    FunctionCallAction<Input(
        KV("numeric type",NumericType),
        KV("imageRange",Range)
    ), KV("positionImage",VariantComplexImage)> processedPositionImageGenerator;

	KernelDefinesAction<
		KV("Type",NumericType),
		KV("FORMULA",std::string),
		KV("JULIAMODE",bool),
		KV("MAXITER",uint32_t),
		KV("BAILOUT",float),
		KV("DISABLE_BAILOUT",bool),
		KV("CYCLE_DETECTION",bool),
		KV("CYCLE_DETECTION_VISUALIZE",bool),
		KV("SMOOTH_MODE",bool),
		KV("SMOOTH_EXP",float),
		KV("STAT_FUNCTION",std::string)> definesAction;
	KernelGeneratorAction<VariantComplexImage,FloatImage, VariantComplexImage,Float4Image,VariantNumericType ,VariantNumericType,AdditionalKernelParams...> kernelGeneratorAction;
	KernelAction<Input(
		KV("positionImage",VariantComplexImage),
		KV("iterationImage",FloatImage),
		KV("processedPositionImage",VariantComplexImage),
		KV("statsImage",Float4Image),
		KV("julia c real",VariantNumericType),
		KV("julia c imag",VariantNumericType),
		AdditionalKernelParams...
	), KernelOutput<1,2,3>> kernelAction;

	virtual ~CalculationActionBase() = default;
protected:
	virtual bool step() = 0;
	virtual void reset()= 0;

	void executeImpl(){
		if(this->getInput(_C("visualize steps")).getValue()){
			if(this->getInput(_C("reset calculation")).getValue()){
				this->reset();
			}
			Timer t;
			t.start();
			this->getOutput(_C("done")).setValue(this->step());
			auto time = t.stop();
			this->getOutput(_C("time")).setValue(time);
		}else{
			this->reset();
			Timer t;
			t.start();
			while(!this->step());
			auto time = t.stop();
			this->getOutput(_C("time")).setValue(time);
			this->getOutput(_C("done")).setValue(true);
		}
	}
};




