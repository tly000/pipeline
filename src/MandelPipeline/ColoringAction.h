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
		this->delegateInput(_C("outer gradient"),this->gradientBufferGenerator1.getInput(_C("data")));
		this->delegateInput(_C("inner gradient"),this->gradientBufferGenerator2.getInput(_C("data")));

		this->delegateInput(_C("outer curve"),this->curveBufferGenerator1.getInput(_C("data")));
		this->delegateInput(_C("inner curve"),this->curveBufferGenerator2.getInput(_C("data")));

		this->delegateInput(_C("outer coloring method"),this->definesAction.getInput(_C("OUTSIDE_COLORING_KERNEL")));
		this->delegateInput(_C("inner coloring method"),this->definesAction.getInput(_C("INSIDE_COLORING_KERNEL")));
		this->delegateInput(_C("bailout"),this->definesAction.getInput(_C("BAILOUT")));
		this->delegateInput(_C("enable juliamode"),this->definesAction.getInput(_C("JULIAMODE")));
		this->delegateInput(_C("iterations"),this->definesAction.getInput(_C("MAXITER")));
		this->definesAction.getInput(_C("Type")).setDefaultValue(typeName);
		this->definesAction.naturalConnect(kernelGeneratorAction);

		kernelGeneratorAction.getInput(_C("programName")).setDefaultValue("coloring");
		kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("coloringKernel");

		kernelGeneratorAction.naturalConnect(kernelAction);

		this->delegateInput(_C("iterationImage"),kernelAction.getInput(_C("iterationImage")));
		this->delegateInput(_C("processedPositionImage"),kernelAction.getInput(_C("processedPositionImage")));
		this->delegateInput(_C("statsImage"),kernelAction.getInput(_C("statsImage")));
		this->delegateInput(_C("coloredImage"), kernelAction.getInput(_C("coloredImage")));
		this->delegateInput(_C("imageRange"), kernelAction.getInput(_C("globalSize")));
		this->gradientBufferGenerator1.template output<0>() >> this->kernelAction.getInput(_C("gradientA"));
		this->gradientBufferGenerator2.template output<0>() >> this->kernelAction.getInput(_C("gradientB"));
		this->curveBufferGenerator1.template output<0>() >> this->kernelAction.getInput(_C("curveA"));
		this->curveBufferGenerator2.template output<0>() >> this->kernelAction.getInput(_C("curveB"));

		this->delegateOutput(_C("coloredImage"), kernelAction.getOutput(_C("coloredImage")));
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
