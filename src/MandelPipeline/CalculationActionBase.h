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
		KV("done",bool)
	)>{
		CalculationActionBase(Factory& factory,std::string typeName)
	  :kernelGeneratorAction(factory){
		this->template delegateInput("processed formula"_c, definesAction.getInput("FORMULA"_c));
		this->template delegateInput("enable juliamode"_c, definesAction.getInput("JULIAMODE"_c));
		this->template delegateInput("iterations"_c, definesAction.getInput("MAXITER"_c));
		this->template delegateInput("bailout"_c, definesAction.getInput("BAILOUT"_c));
		this->template delegateInput("disable bailout"_c, definesAction.getInput("DISABLE_BAILOUT"_c));
		this->template delegateInput("cycle detection"_c, definesAction.template getInput("CYCLE_DETECTION"_c));
		this->template delegateInput("visualize cycle detection"_c, definesAction.getInput("CYCLE_DETECTION_VISUALIZE"_c));
		this->template delegateInput("statistic function"_c, definesAction.getInput("STAT_FUNCTION"_c));

		definesAction.getInput("Type"_c).setDefaultValue(typeName);
		definesAction.naturalConnect(kernelGeneratorAction);
		kernelGeneratorAction.getInput("programName"_c).setDefaultValue("calculation");
		kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("mandelbrotKernel");

		kernelGeneratorAction.naturalConnect(kernelAction);

		this->template delegateInput("positionImage"_c,kernelAction.getInput("positionImage"_c));
		this->template delegateInput("iterationImage"_c, kernelAction.getInput("iterationImage"_c));
		this->template delegateInput("processedPositionImage"_c, kernelAction.getInput("processedPositionImage"_c));
		this->template delegateInput("statsImage"_c, kernelAction.getInput("statsImage"_c));
		this->template delegateInput("julia c real"_c, kernelAction.getInput("julia c real"_c));
		this->template delegateInput("julia c imag"_c, kernelAction.getInput("julia c imag"_c));
		this->template delegateInput("imageRange"_c, kernelAction.getInput("globalSize"_c));

		this->template delegateOutput("iterationImage"_c, kernelAction.getOutput("iterationImage"_c));
		this->template delegateOutput("processedPositionImage"_c, kernelAction.getOutput("processedPositionImage"_c));
		this->template delegateOutput("statsImage"_c, kernelAction.getOutput("statsImage"_c));
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
		if(this->getInput("visualize steps"_c).getValue()){
			if(this->getInput("reset calculation"_c).getValue()){
				this->reset();
			}
			this->getOutput("done"_c).setValue(this->step());
		}else{
			this->reset();
			while(!this->step());
			this->getOutput("done"_c).setValue(true);
		}
	}
};




