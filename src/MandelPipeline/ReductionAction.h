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
		this->delegateInput(_C("enable multisampling"), definesAction.getInput(_C("MULTISAMPLING_ENABLED")));
		this->delegateInput(_C("size"),definesAction.getInput(_C("MULTISAMPLING_SIZE")));

		definesAction.getInput(_C("Type")).setDefaultValue(typeName);
		definesAction.naturalConnect(kernelGeneratorAction);
		kernelGeneratorAction.getInput(_C("programName")).setDefaultValue("reduction");
		kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("reductionKernel");

		kernelGeneratorAction.naturalConnect(kernelAction);

		this->delegateInput(_C("coloredImage"), kernelAction.getInput(_C("coloredImage")));
		this->delegateInput(_C("reducedImage"), kernelAction.getInput(_C("reducedImage")));
		this->delegateInput(_C("imageRange"), kernelAction.getInput(_C("globalSize")));

		this->delegateOutput(_C("reducedImage"),kernelAction.getOutput(_C("reducedImage")));
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
	}
};

