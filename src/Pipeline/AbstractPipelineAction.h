#pragma once
#include <set>
/*
 * AbstractPipelineAction.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

struct AbstractPipelineAction{
	void run();

	virtual ~AbstractPipelineAction() = default;

protected:
	virtual void execute() = 0;

	virtual std::set<AbstractPipelineAction*> getParentActions() = 0;

private:
	void reset();

	void runImpl();

	bool isDone = true;
};
