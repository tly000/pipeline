#pragma once
#include "../Actions/BoxedAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Actions/KernelDefinesAction.h"
#include "../Actions/BufferGeneratorAction.h"
#include "../Type/Curve.h"

/*
 * ColoringAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

template<typename Factory,typename T> struct ColoringAction :
	BoxedAction<Input(
		KV("outer gradient",Gradient),
		KV("outer curve",Curve),
		KV("outer coloring method",std::string),
		KV("inner gradient",Gradient),
		KV("inner curve",Curve),
		KV("inner coloring method",std::string),
		KV("iterations",uint32_t),
		KV("bailout",float),
		KV("enable juliamode",bool),
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("statsImage",Float4Image<Factory>),
		KV("coloredImage",Float3Image<Factory>),
		KV("imageRange",Range)
	),Output(
		KV("coloredImage",Float3Image<Factory>)
	)>{
	ColoringAction(Factory& factory,std::string typeName)
	  :factory(factory),
	   gradientBufferGenerator1(factory),
	   gradientBufferGenerator2(factory),
	   curveBufferGenerator1(factory),
	   curveBufferGenerator2(factory),
	   kernelGeneratorAction(factory){
		this->template delegateInput("outer gradient"_c,this->gradientBufferGenerator1.getInput("data"_c));
		this->template delegateInput("inner gradient"_c,this->gradientBufferGenerator2.getInput("data"_c));

		this->template delegateInput("outer curve"_c,this->curveBufferGenerator1.getInput("data"_c));
		this->template delegateInput("inner curve"_c,this->curveBufferGenerator2.getInput("data"_c));

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
		this->template delegateInput("statsImage"_c,kernelAction.getInput("statsImage"_c));
		this->template delegateInput("coloredImage"_c, kernelAction.getInput("coloredImage"_c));
		this->template delegateInput("imageRange"_c, kernelAction.getInput("globalSize"_c));
		this->gradientBufferGenerator1.template output<0>() >> this->kernelAction.getInput("gradientA"_c);
		this->gradientBufferGenerator2.template output<0>() >> this->kernelAction.getInput("gradientB"_c);
		this->curveBufferGenerator1.template output<0>() >> this->kernelAction.getInput("curveA"_c);
		this->curveBufferGenerator2.template output<0>() >> this->kernelAction.getInput("curveB"_c);

		this->template delegateOutput("coloredImage"_c, kernelAction.getOutput("coloredImage"_c));
	}

	Factory& factory;

	FilledBufferGeneratorAction<Factory,Vec<3,float>>
		gradientBufferGenerator1,
		gradientBufferGenerator2;

	FilledBufferGeneratorAction<Factory,CurveSegment>
		curveBufferGenerator1,
		curveBufferGenerator2;

	KernelDefinesAction<
		KV("Type",std::string),
		KV("MAXITER",uint32_t),
		KV("BAILOUT",float),
		KV("JULIAMODE",bool),
		KV("INSIDE_COLORING_KERNEL",std::string),
		KV("OUTSIDE_COLORING_KERNEL",std::string)
		> definesAction;
	KernelGeneratorAction<Factory,
		FloatImage<Factory>,
		ComplexImage<Factory,T>,
		Float4Image<Factory>,
		Float3Image<Factory>,
		Float3Buffer<Factory>,
		Float3Buffer<Factory>,
		typename Factory::template Buffer<CurveSegment>,
		typename Factory::template Buffer<CurveSegment>> kernelGeneratorAction;
	KernelAction<Factory,Input(
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("statsImage",Float4Image<Factory>),
		KV("coloredImage",Float3Image<Factory>),
		KV("gradientA",Float3Buffer<Factory>),
		KV("gradientB",Float3Buffer<Factory>),
		KV("curveA",typename Factory::template Buffer<CurveSegment>),
		KV("curveB",typename Factory::template Buffer<CurveSegment>)
	), KernelOutput<3>> kernelAction;

protected:
	void executeImpl(){
		kernelAction.run();
	}
};
