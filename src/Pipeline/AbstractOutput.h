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

	virtual void connectTo(AbstractInput& slot);

	virtual void operator>>(AbstractInput&);

	const std::string name;
protected:
	AbstractPipelineAction* const pipeline;
	std::list<AbstractInput*> inputSlots;

	friend AbstractInput;
};

inline void operator>>(const OutputPack<1>& outputPack,AbstractInput& input){
	outputPack.at(0)->connectTo(input);
}

inline void operator>>(AbstractOutput& output,const InputPack<1>& inputPack){
	output.connectTo(*inputPack.at(0));
}




