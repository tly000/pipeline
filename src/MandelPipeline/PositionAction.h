#pragma once
#include "../Actions/BoxedAction.h"
#include "../Actions/KernelAction.h"
#include "../Actions/KernelGeneratorAction.h"
#include "../Actions/KernelDefinesAction.h"
#include "../Type/Vec.h"

/*
 * PositionAction.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

template<typename Factory,typename T,
         typename ComplexImage = typename Factory::template Image<Vec<2,T>>> struct PositionAction :
	BoxedAction<Input(
		KV("center real",T),
		KV("center imag",T),
		KV("scale",T),
		KV("angle",float),
		KV("positionImage",ComplexImage),
		KV("enable multisampling",bool),
		KV("pattern",MultisamplingPattern),
		KV("imageRange",Range)
	),Output(
		KV("positionImage",ComplexImage)
	)>{
	PositionAction(Factory& factory,std::string typeName)
	  :kernelGeneratorAction(factory){
		this->delegateInput(_C("enable multisampling"), definesAction.getInput(_C("MULTISAMPLING_ENABLED")));
		this->delegateInput(_C("pattern"), definesAction.getInput(_C("MULTISAMPLING_PATTERN")));

		definesAction.getInput(_C("Type")).setDefaultValue(typeName);
		definesAction.naturalConnect(kernelGeneratorAction);
		kernelGeneratorAction.getInput(_C("programName")).setDefaultValue("position");
		kernelGeneratorAction.getInput(_C("kernelName")).setDefaultValue("positionKernel");

		this->delegateInput(_C("scale"), rotationAction.getInput(_C("scale")));
		this->delegateInput(_C("angle"), rotationAction.getInput(_C("angle")));
		rotationAction.naturalConnect(kernelAction);

		kernelGeneratorAction.naturalConnect(kernelAction);
		this->delegateInput(_C("positionImage"),kernelAction.getInput(_C("positionImage")));
		this->delegateInput(_C("center real"), kernelAction.getInput(_C("center real")));
		this->delegateInput(_C("center imag"), kernelAction.getInput(_C("center imag")));
		this->delegateInput(_C("imageRange"), kernelAction.getInput(_C("globalSize")));

		this->delegateOutput(_C("positionImage"), kernelAction.getOutput(_C("positionImage")));
	}

	KernelDefinesAction<KV("Type",std::string),KV("MULTISAMPLING_ENABLED",bool),KV("MULTISAMPLING_PATTERN",MultisamplingPattern)> definesAction;
	KernelGeneratorAction<Factory,ComplexImage,T,T,Vec<2,T>> kernelGeneratorAction;
	KernelAction<Factory,Input(
		KV("positionImage",ComplexImage),
		KV("center real",T),
		KV("center imag",T),
		KV("rotation",Vec<2,T>)
	), KernelOutput<0>> kernelAction;

	FunctionCallAction<Input(
		KV("scale",T),
		KV("angle",float)
	), KV("rotation",Vec<2,T>)> rotationAction{
		[](const T& scale, float angle){
			angle *= 3.14159 / 180;
			return Vec<2,T>{
				tmul(scale,fromFloatToType<T>(cosf(angle))),
				tmul(scale,fromFloatToType<T>(sinf(angle)))
			};
		}
	};

protected:
	void executeImpl(){
		kernelAction.run();
	}
};



