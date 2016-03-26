#pragma once
#include <list>
/*
 * AbstractPipelineAction.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

struct AbstractInput;
struct AbstractOutput;

struct AbstractPipelineAction{
	void run();

	virtual ~AbstractPipelineAction() = default;

protected:
	virtual void execute() = 0;

	std::list<AbstractInput*> inputs;
	std::list<AbstractOutput*> outputs;

	friend AbstractInput;
	friend AbstractOutput;
private:
	void reset();

	void runImpl();

	bool isDone = true;
};
