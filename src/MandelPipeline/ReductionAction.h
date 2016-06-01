#pragma once
#include "../Actions/BoxedAction.h"
#include "../Type/Vec.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Actions/KernelDefinesAction.h"

/*
 * ReductionAction.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

template<typename Factory> struct ReductionAction : BoxedAction<Input(
	KV("enable multisampling",bool),
	KV("size",uint32_t),
	KV("coloredImage",typename Factory::template Image<Vec<3,float>>),
	KV("reducedImage",typename Factory::template Image<uint32_t>),
	KV("imageRange",Range)
),Output(
	KV("reducedImage",typename Factory::template Image<uint32_t>)
)>{
	using Float3Image = typename Factory::template Image<Vec<3,float>>;
	using RGBAImage = typename Factory::template Image<uint32_t>;

	ReductionAction(Factory& factory,std::string typeName)
	  :kernelGeneratorAction(factory){
		this->template delegateInput("enable multisampling"_c, definesAction.template getInput("MULTISAMPLING_ENABLED"_c));
		this->template delegateInput("size"_c,definesAction.template getInput("MULTISAMPLING_SIZE"_c));

		definesAction.getInput("Type"_c).setDefaultValue(typeName);
		definesAction.naturalConnect(kernelGeneratorAction);
		kernelGeneratorAction.getInput("programName"_c).setDefaultValue("reduction");
		kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("reductionKernel");

		kernelGeneratorAction.naturalConnect(kernelAction);

		this->template delegateInput("coloredImage"_c, kernelAction.getInput("coloredImage"_c));
		this->template delegateInput("reducedImage"_c, kernelAction.getInput("reducedImage"_c));
		this->template delegateInput("imageRange"_c, kernelAction.getInput("globalSize"_c));

		//this->template delegateOutput("reducedImage"_c,kernelAction.getOutput("reducedImage"_c));
		this->template delegateOutput<0>(kernelAction.getOutput("reducedImage"_c));
	}

	KernelDefinesAction<KV("Type",std::string),KV("MULTISAMPLING_ENABLED",bool),KV("MULTISAMPLING_SIZE",uint32_t)> definesAction;
	KernelGeneratorAction<Factory,Float3Image,RGBAImage> kernelGeneratorAction;
	KernelAction<Factory,Input(
		KV("coloredImage",Float3Image),
		KV("reducedImage",RGBAImage)
	), KernelOutput<1>> kernelAction;

protected:
	void executeImpl(){
		kernelAction.run();
		_log("[info] reduction: " << kernelAction.template getOutput("time"_c).getValue() << " us.");
	}
};

