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

	bool hasValue() const;

	AbstractPipelineAction* getPipeline() const;

	virtual ~AbstractInput();
protected:
	AbstractPipelineAction* const pipeline;
	AbstractOutput* outputSlot = nullptr;

	void connectTo(AbstractOutput& slot);

	friend AbstractOutput;
};


