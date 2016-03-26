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

struct AbstractOutput /*: NonCopyable*/{
	AbstractOutput(AbstractPipelineAction* pipeline);

	AbstractPipelineAction* getPipeline() const;

	virtual ~AbstractOutput();
protected:
	AbstractPipelineAction* const pipeline;
	std::list<AbstractInput*> inputSlots;

	void connectTo(AbstractInput& slot);

	friend AbstractInput;
};



