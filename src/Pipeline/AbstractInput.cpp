#include "AbstractInput.h"
#include "AbstractOutput.h"
#include "../Utility/Utils.h"

/*
 * AbstractInput.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

void AbstractInput::connectTo(AbstractOutput& slot) {
	slot.connectTo(*this);
}

AbstractOutput* AbstractInput::getOutputSlot() {
	return outputSlot;
}

const AbstractOutput* AbstractInput::getOutputSlot() const {
	return outputSlot;
}

AbstractPipelineAction* AbstractInput::getPipeline() const {
	return pipeline;
}

AbstractInput::AbstractInput(AbstractPipelineAction* pipeline,std::string name)
	:name(name),
	 pipeline(pipeline){
	pipeline->inputs.push_back(this);
}

AbstractInput::~AbstractInput() {
	eraseFromList(pipeline->inputs,this);
}

bool AbstractInput::isConnected() {
	return getOutputSlot();
}

bool AbstractInput::hasValue() const {
	return this->getOutputSlot() && this->getOutputSlot()->hasValue();
}
