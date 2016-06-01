#pragma once
#include "../Actions/BoxedAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Actions/KernelDefinesAction.h"

/*
 * ColoringAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

template<typename Factory,typename T,
		 typename FloatImage = typename Factory::template Image<float>,
		 typename Float3Image= typename Factory::template Image<Vec<3,float>>> struct ColoringAction :
	BoxedAction<Input(
		KV("iterations",uint32_t),
		KV("iterationImage",FloatImage),
		KV("coloredImage",Float3Image),
		KV("imageRange",Range)
	),Output(
		KV("coloredImage",Float3Image)
	)>{
	ColoringAction(Factory& factory,std::string typeName)
	  :kernelGeneratorAction(factory){
		this->template delegateInput("iterations"_c,definesAction.getInput("MAXITER"_c));
		definesAction.getInput("Type"_c).setDefaultValue(typeName);
		definesAction.naturalConnect(kernelGeneratorAction);

		kernelGeneratorAction.getInput("programName"_c).setDefaultValue("coloring");
		kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("coloringKernel");

		kernelGeneratorAction.naturalConnect(kernelAction);

		this->template delegateInput("iterationImage"_c,kernelAction.getInput("iterationImage"_c));
		this->template delegateInput("coloredImage"_c, kernelAction.getInput("coloredImage"_c));
		this->template delegateInput("imageRange"_c, kernelAction.getInput("globalSize"_c));

		//this->template delegateOutput("coloredImage"_c, kernelAction.getOutput("coloredImage"_c));
		this->template delegateOutput<0>(kernelAction.getOutput("coloredImage"_c));
	}

	KernelDefinesAction<KV("Type",std::string),KV("MAXITER",uint32_t)> definesAction;
	KernelGeneratorAction<Factory,FloatImage,Float3Image> kernelGeneratorAction;
	KernelAction<Factory,Input(
		KV("iterationImage",FloatImage),
		KV("coloredImage",Float3Image)
	), KernelOutput<1>> kernelAction;

protected:
	void executeImpl(){
		kernelAction.run();
		_log("[info] coloring: " << kernelAction.template getOutput("time"_c).getValue() << " us.");
	}
};
