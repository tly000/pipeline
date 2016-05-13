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
		:AbstractInput(pipeline),
		 defaultValue(nullptr){}

	T& getValue() const;

	virtual void connectTo(AbstractOutput& slot);

	void setDefaultValue(const T&);

	virtual bool hasValue() const;
protected:
	std::unique_ptr<T> defaultValue;
};

template<typename T>
struct StaticOutput : AbstractOutput{
	StaticOutput(AbstractPipelineAction* pipeline)
		:AbstractOutput(pipeline),
		 data(nullptr){}

	virtual void connectTo(AbstractInput& slot);

	const T& getValue() const;

	T& getValue();

	void setValue(const T&);

	virtual bool hasValue() const;
protected:
	std::unique_ptr<T> data;
};

template<typename T>
inline T& StaticInput<T>::getValue() const {
	if(outputSlot){
		return static_cast<StaticOutput<T>*>(outputSlot)->getValue();
	}else if(defaultValue){
		return *defaultValue;
	}else{
		throw std::runtime_error("slot not connected.");
	}
}

template<typename T>
inline const T& StaticOutput<T>::getValue() const {
	if(data.get()){
		return *data;
	} else {
		throw std::runtime_error("output contains no value.");
	}
}

template<typename T>
inline void StaticOutput<T>::connectTo(AbstractInput& slot) {
	if(dynamic_cast<StaticInput<T>*>(&slot)){
		AbstractOutput::connectTo(slot);
	} else {
		std::cout << demangle(typeid(*this)) << std::endl;
		throw std::runtime_error("connected slot type is not compatible");
	}
}

template<typename T>
inline T& StaticOutput<T>::getValue() {
	if(data.get()){
		return *data;
	} else {
		throw std::runtime_error("output contains no value.");
	}
}

template<typename T>
inline void StaticInput<T>::connectTo(AbstractOutput& slot) {
	if(dynamic_cast<StaticOutput<T>*>(&slot)){
		AbstractInput::connectTo(slot);
	} else {
		throw std::runtime_error("connected slot type is not compatible");
	}
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

template<typename T>
inline void StaticInput<T>::setDefaultValue(const T& val) {
	this->defaultValue = std::make_unique<T>(val);
}

template<typename T>
inline bool StaticInput<T>::hasValue() const {
	return this->defaultValue || AbstractInput::hasValue();
}
