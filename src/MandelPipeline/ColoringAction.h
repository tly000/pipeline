#pragma once
#include "../Actions/BoxedAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Actions/KernelDefinesAction.h"
#include "../Actions/BufferGeneratorAction.h"

/*
 * ColoringAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct ColoringAction :
	BoxedAction<Input(
		KV("smooth iteration count",bool),
		KV("leading polynomial exponent",float),
		KV("outer gradient",Gradient),
		KV("outer coloring method",std::string),
		KV("inner gradient",Gradient),
		KV("inner coloring method",std::string),
		KV("iterations",uint32_t),
		KV("bailout",float),
		KV("enable juliamode",bool),
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("coloredImage",Float3Image<Factory>),
		KV("imageRange",Range)
	),Output(
		KV("coloredImage",Float3Image<Factory>)
	)>{
	ColoringAction(Factory& factory,std::string typeName)
	  :factory(factory),
	   gradientBufferGenerator1(factory),
	   gradientBufferGenerator2(factory),
	   kernelGeneratorAction(factory){
		this->template delegateInput("outer gradient"_c,this->gradientBufferGenerator1.getInput("data"_c));
		this->template delegateInput("inner gradient"_c,this->gradientBufferGenerator2.getInput("data"_c));

		this->template delegateInput("smooth iteration count"_c,this->definesAction.getInput("SMOOTH_MODE"_c));
		this->template delegateInput("leading polynomial exponent"_c,this->definesAction.getInput("SMOOTH_EXP"_c));
		this->template delegateInput("outer coloring method"_c,this->definesAction.getInput("OUTSIDE_COLORING_KERNEL"_c));
		this->template delegateInput("inner coloring method"_c,this->definesAction.getInput("INSIDE_COLORING_KERNEL"_c));
		this->template delegateInput("bailout"_c,this->definesAction.getInput("BAILOUT"_c));
		this->template delegateInput("enable juliamode"_c,this->definesAction.getInput("JULIAMODE"_c));
		this->template delegateInput("iterations"_c,this->definesAction.getInput("MAXITER"_c));
		this->definesAction.getInput("Type"_c).setDefaultValue(typeName);
		this->definesAction.naturalConnect(kernelGeneratorAction);

		kernelGeneratorAction.getInput("programName"_c).setDefaultValue("coloring");
		kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("coloringKernel");

		kernelGeneratorAction.naturalConnect(kernelAction);

		this->template delegateInput("iterationImage"_c,kernelAction.getInput("iterationImage"_c));
		this->template delegateInput("processedPositionImage"_c,kernelAction.getInput("processedPositionImage"_c));
		this->template delegateInput("coloredImage"_c, kernelAction.getInput("coloredImage"_c));
		this->template delegateInput("imageRange"_c, kernelAction.getInput("globalSize"_c));
		this->gradientBufferGenerator1.template output<0>() >> this->kernelAction.getInput("gradientA"_c);
		this->gradientBufferGenerator2.template output<0>() >> this->kernelAction.getInput("gradientB"_c);

		this->template delegateOutput("coloredImage"_c, kernelAction.getOutput("coloredImage"_c));

		this->definesAction.getInput("SMOOTH_MODE"_c).setDefaultValue(1);
	}

	Factory& factory;

	FilledBufferGeneratorAction<Factory,Vec<3,float>>
		gradientBufferGenerator1,
		gradientBufferGenerator2;

	KernelDefinesAction<
		KV("Type",std::string),
		KV("MAXITER",uint32_t),
		KV("BAILOUT",float),
		KV("JULIAMODE",bool),
		KV("SMOOTH_MODE",uint32_t),
		KV("SMOOTH_EXP",float),
		KV("INSIDE_COLORING_KERNEL",std::string),
		KV("OUTSIDE_COLORING_KERNEL",std::string)
		> definesAction;
	KernelGeneratorAction<Factory,
		FloatImage<Factory>,
		ComplexImage<Factory,T>,
		Float3Image<Factory>,
		Float3Buffer<Factory>,
		Float3Buffer<Factory>> kernelGeneratorAction;
	KernelAction<Factory,Input(
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("coloredImage",Float3Image<Factory>),
		KV("gradientA",Float3Buffer<Factory>),
		KV("gradientB",Float3Buffer<Factory>)
	), KernelOutput<2>> kernelAction;

protected:
	void executeImpl(){
		kernelAction.run();
	}
};
