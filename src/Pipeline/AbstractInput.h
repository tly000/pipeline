#pragma once
#include "AbstractPipelineAction.h"
#include "../Utility/NonCopyable.h"

/*
 * AbstractInput.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

struct AbstractOutput;

struct AbstractInput : NonCopyable{
	AbstractInput(AbstractPipelineAction* pipeline);

	AbstractOutput* getOutputSlot();
	const AbstractOutput* getOutputSlot() const;

	bool isConnected();

	virtual bool hasValue() const;

	AbstractPipelineAction* getPipeline() const;

	virtual ~AbstractInput();

	virtual void connectTo(AbstractOutput& slot);
protected:
	AbstractPipelineAction* const pipeline;
	AbstractOutput* outputSlot = nullptr;

	friend AbstractOutput;
};


