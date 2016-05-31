#pragma once
#include "../Utility/NonCopyable.h"
#include "AbstractPipelineAction.h"
#include <list>

/*
 * AbstractOutput.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */
struct AbstractInput;

struct AbstractOutput : NonCopyable{
	AbstractOutput(AbstractPipelineAction* pipeline,std::string name);

	AbstractPipelineAction* getPipeline() const;

	virtual bool hasValue() const = 0;

	virtual ~AbstractOutput();

	const std::string name;
protected:
	void connectTo(AbstractInput& slot);

	AbstractPipelineAction* const pipeline;
	std::list<AbstractInput*> inputSlots;

	friend AbstractInput;
};
