#include "AbstractPipelineAction.h"
#include "AbstractInput.h"
#include "AbstractOutput.h"

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
		for(auto& input : this->inputs){
			if(auto* output = input->getOutputSlot()){
				output->getPipeline()->runImpl();
			}
		}
		this->execute();
		this->isDone = true;
	}
}




