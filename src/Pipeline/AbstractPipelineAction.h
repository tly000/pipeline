#pragma once
#include <list>
#include <cstdint>
#include <array>
#include "../Utility/VariadicUtils.h"
/*
 * AbstractPipelineAction.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

struct AbstractInput;
struct AbstractOutput;

template<size_t N> using InputPack = std::array<AbstractInput*,N>;
template<size_t N> using OutputPack = std::array<AbstractOutput*,N>;

struct AbstractPipelineAction{
	void run();

	virtual ~AbstractPipelineAction() = default;

	template<typename... I> InputPack<sizeof...(I)> input(I... i);
	template<typename... I> OutputPack<sizeof...(I)> output(I... i);

protected:
	virtual void execute() = 0;

	std::list<AbstractInput*> inputs;
	std::list<AbstractOutput*> outputs;

	friend AbstractInput;
	friend AbstractOutput;

	virtual void reset();
	virtual void runImpl();

	bool isDone = true;
};

template<size_t N> void operator>>(const OutputPack<N>& outputPack,const InputPack<N>& inputPack){
	for(unsigned i = 0; i < N; i++){
		outputPack.at(i)->connectTo(*inputPack.at(i));
	}
}
template<typename ... I>
inline InputPack<sizeof...(I)> AbstractPipelineAction::input(I ... i) {
	variadicForEach(size_t(i) < this->inputs.size() ? true : throw std::runtime_error("input index " + std::to_string(i) + " out of range"));
	return std::array<AbstractInput*,sizeof...(I)>{
		*std::next(this->inputs.begin(),i)...
	};
}

template<typename ... I>
inline OutputPack<sizeof...(I)> AbstractPipelineAction::output(I ... i) {
	variadicForEach(size_t(i) < this->outputs.size() ? true : throw std::runtime_error("output index " + std::to_string(i) + " out of range"));
	return std::array<AbstractOutput*,sizeof...(I)>{
		*std::next(this->outputs.begin(),i)...
	};
}
