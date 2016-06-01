#include "AbstractPipelineAction.h"
#include "AbstractInput.h"
#include "AbstractOutput.h"
#include "../Utility/Utils.h"

/*
 * AbstractPipelineAction.cpp
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

void AbstractPipelineAction::run() {
	this->reset();
	this->runImpl();
}

void AbstractPipelineAction::reset() {
	if(this->isDone){
		this->isDone = false;
		for(auto& input : this->inputs){
			if(auto* output = input->getOutputSlot()){
				output->getPipeline()->reset();
			}
		}
	}
}

void AbstractPipelineAction::runImpl() {
	if(!this->isDone){
		int i = 0;
		for(auto& input : this->inputs){
			if(auto* output = input->getOutputSlot()){
				output->getPipeline()->runImpl();
			} else if(!input->hasValue()) {
				_log("[warning] slot " << i << "( " << input->name << ")" << " of action " << demangle(typeid(*this)) << " not connected.");
			}
			i++;
		}
		_logDebug("executing " << demangle(typeid(*this)));
		this->execute();
		this->isDone = true;
	}
}
