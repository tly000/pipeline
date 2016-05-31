#pragma once
/*
 * PipelineWrapper.h
 *
 *  Created on: May 31, 2016
 *      Author: tly
 */

#pragma once
#include "../UI/UIParameterAction.h"
#include "../Pipeline/AbstractPipelineAction.h"
#include <sstream>
#include "../Parser/JsonParser.h"

/*
 * PipelineWrapper.h
 *
 *  Created on: 03.05.2016
 *      Author: tly
 */

struct PipelineWrapper : NonCopyable{
	virtual void run() = 0;

	virtual ~PipelineWrapper() = default;
protected:
	void addParam(ParamPack& paramPack){
		this->paramPackMap.insert({
			paramPack.name,
			&paramPack
		});
	}

	std::map<std::string,ParamPack*> paramPackMap;
public:
	const std::map<std::string,ParamPack*>& getParamPacks(){
		return this->paramPackMap;
	}

	ParamPack& getParamPack(std::string name){
		return *paramPackMap.at(name);
	}
};
