#pragma once
#include "../Actions/BoxedAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Actions/KernelDefinesAction.h"
#include "../Type/Vec.h"

/*
 * CalculationAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

template<typename Factory,typename T,
         typename ComplexImage = typename Factory::template Image<Vec<2,T>>,
		 typename FloatImage = typename Factory::template Image<float>> struct CalculationAction :
	BoxedAction<Input(
		KV("processed formula",std::string),
		KV("enable juliamode",bool),
		KV("julia c real",T),
		KV("julia c imag",T),
		KV("iterations",uint32_t),
		KV("bailout",float),
		KV("cycle detection",bool),
		KV("visualize cycle detection",bool),
		KV("positionImage",ComplexImage),
		KV("iterationImage",FloatImage),
		KV("imageRange",Range)
	),Output(
		KV("iterationImage",FloatImage)
	)>{
	CalculationAction(Factory& factory,std::string typeName)
	  :kernelGeneratorAction(factory){
		this->template delegateInput("processed formula"_c, definesAction.getInput("FORMULA"_c));
		this->template delegateInput("enable juliamode"_c, definesAction.getInput("JULIAMODE"_c));
		this->template delegateInput("iterations"_c, definesAction.getInput("MAXITER"_c));
		this->template delegateInput("bailout"_c, definesAction.getInput("BAILOUT"_c));
		this->template delegateInput("cycle detection"_c, definesAction.template getInput("CYCLE_DETECTION"_c));
		this->template delegateInput("visualize cycle detection"_c, definesAction.getInput("CYCLE_DETECTION_VISUALIZE"_c));

		definesAction.getInput("Type"_c).setDefaultValue(typeName);
		definesAction.naturalConnect(kernelGeneratorAction);
		kernelGeneratorAction.getInput("programName"_c).setDefaultValue("calculation");
		kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("mandelbrotKernel");

		kernelGeneratorAction.naturalConnect(kernelAction);

		this->template delegateInput("positionImage"_c,kernelAction.getInput("positionImage"_c));
		this->template delegateInput("iterationImage"_c, kernelAction.getInput("iterationImage"_c));
		this->template delegateInput("julia c real"_c, kernelAction.getInput("julia c real"_c));
		this->template delegateInput("julia c imag"_c, kernelAction.getInput("julia c imag"_c));
		this->template delegateInput("imageRange"_c, kernelAction.getInput("globalSize"_c));

		//this->template delegateOutput("iterationImage"_c, kernelAction.getOutput("iterationImage"_c));
		this->template delegateOutput<0>(kernelAction.getOutput("iterationImage"_c));
	}

	KernelDefinesAction<
		KV("Type",std::string),
		KV("FORMULA",std::string),
		KV("JULIAMODE",bool),
		KV("MAXITER",uint32_t),
		KV("BAILOUT",float),
		KV("CYCLE_DETECTION",bool),
		KV("CYCLE_DETECTION_VISUALIZE",bool)> definesAction;
	KernelGeneratorAction<Factory,ComplexImage,FloatImage,T,T> kernelGeneratorAction;
	KernelAction<Factory,Input(
		KV("positionImage",ComplexImage),
		KV("iterationImage",FloatImage),
		KV("julia c real",T),
		KV("julia c imag",T)
	), KernelOutput<1>> kernelAction;

	virtual ~CalculationAction() = default;
protected:
	virtual void executeImpl(){
		kernelAction.run();
		_log("[info] calculation: " << kernelAction.template getOutput("time"_c).getValue() << " us.");
	}
};




