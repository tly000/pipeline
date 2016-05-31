#include "AbstractOutput.h"
#include "AbstractInput.h"
#include "../Utility/Utils.h"

/*
 * AbstractOutput.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

void AbstractOutput::connectTo(AbstractInput& slot) {
	if(slot.getOutputSlot()){
		eraseFromList(slot.outputSlot->inputSlots,&slot);
	}
	slot.outputSlot = this;
	this->inputSlots.push_back(&slot);
}

AbstractPipelineAction* AbstractOutput::getPipeline() const {
	return pipeline;
}

AbstractOutput::AbstractOutput(AbstractPipelineAction* pipeline,std::string name)
	:name(name),
	 pipeline(pipeline){
	pipeline->outputs.push_back(this);
}

AbstractOutput::~AbstractOutput() {
	eraseFromList(pipeline->outputs,this);
}
