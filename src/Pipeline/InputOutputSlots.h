#pragma once
#include "AbstractPipelineAction.h"
#include <list>
#include "../Utility/NonCopyable.h"
#include "../Utility/Utils.h"

/*
 * InputOutputSlots.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

template<typename T> struct InputSlot;
template<typename T> struct OutputSlot;

template<typename T>
struct InputSlot : NonCopyable{
	InputSlot(AbstractPipelineAction* pipeline)
		:pipeline(pipeline){}

	void connectTo(OutputSlot<T>& slot);

	const T& getValue() const;

	OutputSlot<T>* getOutputSlot();
protected:
	AbstractPipelineAction* const pipeline;
	OutputSlot<T>* outputSlot = nullptr;

	friend OutputSlot<T>;
};

template<typename T>
struct OutputSlot : NonCopyable{
	OutputSlot(AbstractPipelineAction* pipeline,T data = T())
		:pipeline(pipeline),
		 data(data){}

	void connectTo(InputSlot<T>& slot);

	const T& getValue() const;

	T& getValue();

	AbstractPipelineAction* getPipeline();
protected:
	AbstractPipelineAction* const pipeline;
	T data;
	std::list<InputSlot<T>*> inputSlots;
};

template<typename T>
inline void InputSlot<T>::connectTo(OutputSlot<T>& slot) {
	slot.connectTo(*this);
}

template<typename T>
inline const T& InputSlot<T>::getValue() const {
	if(outputSlot){
		return outputSlot->getValue();
	}else{
		throw std::runtime_error("slot not connected.");
	}
}

template<typename T>
inline void OutputSlot<T>::connectTo(InputSlot<T>& slot) {
	if(slot.getOutputSlot()){
		eraseFromList(slot.outputSlot->inputSlots,&slot);
	}
	slot.outputSlot = this;
	this->inputSlots.push_back(&slot);
}

template<typename T>
inline const T& OutputSlot<T>::getValue() const {
	return data;
}

template<typename T>
inline T& OutputSlot<T>::getValue() {
	return data;
}

template<typename T>
inline OutputSlot<T>* InputSlot<T>::getOutputSlot() {
	return outputSlot;
}

template<typename T>
inline AbstractPipelineAction* OutputSlot<T>::getPipeline() {
	return pipeline;
}
