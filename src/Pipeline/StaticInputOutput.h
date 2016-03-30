#pragma once
#include "AbstractPipelineAction.h"
#include <list>
#include "../Utility/NonCopyable.h"
#include "../Utility/Utils.h"
#include "AbstractInput.h"
#include "AbstractOutput.h"

/*
 * InputOutputSlots.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

template<typename T> struct StaticInput;
template<typename T> struct StaticOutput;

template<typename T>
struct StaticInput : AbstractInput{
	StaticInput(AbstractPipelineAction* pipeline)
		:AbstractInput(pipeline){}

	const T& getValue() const;

	void connectTo(StaticOutput<T>& slot);
};

template<typename T>
struct StaticOutput : AbstractOutput{
	StaticOutput(AbstractPipelineAction* pipeline)
		:AbstractOutput(pipeline),
		 data(nullptr){}

	void connectTo(StaticInput<T>& slot);

	const T& getValue() const;

	T& getValue();

	void setValue(const T&);

	virtual bool hasValue() const;
protected:
	std::unique_ptr<T> data;
};

template<typename T>
inline const T& StaticInput<T>::getValue() const {
	if(outputSlot){
		return static_cast<StaticOutput<T>*>(outputSlot)->getValue();
	}else{
		throw std::runtime_error("slot not connected.");
	}
}

template<typename T>
inline const T& StaticOutput<T>::getValue() const {
	return data;
}

template<typename T>
inline void StaticOutput<T>::connectTo(StaticInput<T>& slot) {
	AbstractOutput::connectTo(slot);
}

template<typename T>
inline T& StaticOutput<T>::getValue() {
	return *data;
}

template<typename T>
inline void StaticInput<T>::connectTo(StaticOutput<T>& slot) {
	AbstractInput::connectTo(slot);
}

template<typename T>
inline void StaticOutput<T>::setValue(const T& val) {
	if(this->hasValue()){
		this->getValue() = val;
	} else {
		this->data = std::make_unique<T>(val);
	}
}

template<typename T>
inline bool StaticOutput<T>::hasValue() const {
	return data.get() != nullptr;
}
