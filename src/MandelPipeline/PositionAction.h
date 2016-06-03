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
		this->template delegateInput("enable multisampling"_c, definesAction.template getInput("MULTISAMPLING_ENABLED"_c));
		this->template delegateInput("pattern"_c, definesAction.template getInput("MULTISAMPLING_PATTERN"_c));

		definesAction.getInput("Type"_c).setDefaultValue(typeName);
		definesAction.naturalConnect(kernelGeneratorAction);
		kernelGeneratorAction.getInput("programName"_c).setDefaultValue("position");
		kernelGeneratorAction.getInput("kernelName"_c).setDefaultValue("positionKernel");

		this->template delegateInput("scale"_c, rotationAction.template getInput("scale"_c));
		this->template delegateInput("angle"_c, rotationAction.template getInput("angle"_c));
		rotationAction.naturalConnect(kernelAction);

		kernelGeneratorAction.naturalConnect(kernelAction);
		this->template delegateInput("positionImage"_c,kernelAction.getInput("positionImage"_c));
		this->template delegateInput("center real"_c, kernelAction.getInput("center real"_c));
		this->template delegateInput("center imag"_c, kernelAction.getInput("center imag"_c));
		this->template delegateInput("imageRange"_c, kernelAction.getInput("globalSize"_c));

		this->template delegateOutput("positionImage"_c, kernelAction.getOutput("positionImage"_c));
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



