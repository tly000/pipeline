#pragma once
#include "AbstractPipelineAction.h"
#include "../Utility/NonCopyable.h"
#include <string>

/*
 * AbstractInput.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

struct AbstractOutput;

struct AbstractInput : NonCopyable{
	AbstractInput(AbstractPipelineAction* pipeline,std::string name);

	AbstractOutput* getOutputSlot();
	const AbstractOutput* getOutputSlot() const;

	bool isConnected();

	virtual bool hasValue() const;

	AbstractPipelineAction* getPipeline() const;

	virtual ~AbstractInput();

	void disconnect();

	const std::string name;
protected:
	void connectTo(AbstractOutput& slot);

	AbstractPipelineAction* const pipeline;
	AbstractOutput* outputSlot = nullptr;

	friend AbstractOutput;
};


