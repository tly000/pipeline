#include "AbstractPipelineAction.h"

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
		for(auto& action : this->getParentActions()){
			action->reset();
		}
	}
}

void AbstractPipelineAction::runImpl() {
	if(!this->isDone){
		for(auto& action : this->getParentActions()){
			action->runImpl();
		}
		this->execute();
		this->isDone = true;
	}
}




