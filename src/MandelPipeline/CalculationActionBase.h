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

template<typename Factory> using UInt2Buffer = typename Factory::template Buffer<Vec<2,uint32_t>>;
template<typename Factory> using UIntBuffer = typename Factory::template Buffer<uint32_t>;
template<typename Factory> using UCharBuffer = typename Factory::template Buffer<uint8_t>;
template<typename Factory> using Float3Buffer = typename Factory::template Buffer<Vec<3,float>>;
template<typename Factory,typename T> using ComplexImage = typename Factory::template Image<Vec<2,T>>;
template<typename Factory> using FloatImage = typename Factory::template Image<float>;
template<typename Factory> using Float3Image = typename Factory::template Image<Vec<3,float>>;
template<typename Factory> using Float4Image = typename Factory::template Image<Vec<4,float>>;
template<typename Factory> using RGBAImage = typename Factory::template Image<uint32_t>;

template<typename Factory,typename T,typename... AdditionalKernelParams> struct CalculationActionBase :
	BoxedAction<Input(
		KV("visualize steps",bool),
		KV("reset calculation",bool),
		KV("processed formula",std::string),
		KV("enable juliamode",bool),
		KV("julia c real",T),
		KV("julia c imag",T),
		KV("iterations",uint32_t),
		KV("bailout",float),
		KV("disable bailout",bool),
		KV("cycle detection",bool),
		KV("visualize cycle detection",bool),
		KV("smooth iteration count",bool),
		KV("leading polynomial exponent",float),
		KV("statistic function",std::string),
		KV("positionImage",ComplexImage<Factory,T>),
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("statsImage",Float4Image<Factory>),
		KV("imageRange",Range)
	),Output(
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("statsImage",Float4Image<Factory>),
		KV("time",uint64_t),
		KV("done",bool)
	)>{
		CalculationActionBase(Factory& factory,std::string typeName)
	  :kernelGeneratorAction(factory){
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

		definesAction.getInput(_C("Type")).setDefaultValue(typeName);
		definesAction.naturalConnect(kernelGeneratorAction);
		kernelGeneratorAction.getInput(_C("programName")).setDefaultValue("calculation");
		kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("mandelbrotKernel");

		kernelGeneratorAction.naturalConnect(kernelAction);

		this->delegateInput(_C("positionImage"),kernelAction.getInput(_C("positionImage")));
		this->delegateInput(_C("iterationImage"), kernelAction.getInput(_C("iterationImage")));
		this->delegateInput(_C("processedPositionImage"), kernelAction.getInput(_C("processedPositionImage")));
		this->delegateInput(_C("statsImage"), kernelAction.getInput(_C("statsImage")));
		this->delegateInput(_C("julia c real"), kernelAction.getInput(_C("julia c real")));
		this->delegateInput(_C("julia c imag"), kernelAction.getInput(_C("julia c imag")));
		this->delegateInput(_C("imageRange"), kernelAction.getInput(_C("globalSize")));

		this->delegateOutput(_C("iterationImage"), kernelAction.getOutput(_C("iterationImage")));
		this->delegateOutput(_C("processedPositionImage"), kernelAction.getOutput(_C("processedPositionImage")));
		this->delegateOutput(_C("statsImage"), kernelAction.getOutput(_C("statsImage")));
	}

	KernelDefinesAction<
		KV("Type",std::string),
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
	KernelGeneratorAction<Factory,ComplexImage<Factory,T>,FloatImage<Factory>,ComplexImage<Factory,T>,Float4Image<Factory>,T,T,AdditionalKernelParams...> kernelGeneratorAction;
	KernelAction<Factory,Input(
		KV("positionImage",ComplexImage<Factory,T>),
		KV("iterationImage",FloatImage<Factory>),
		KV("processedPositionImage",ComplexImage<Factory,T>),
		KV("statsImage",Float4Image<Factory>),
		KV("julia c real",T),
		KV("julia c imag",T),
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




