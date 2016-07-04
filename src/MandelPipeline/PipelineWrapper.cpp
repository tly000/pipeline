/*
 * PipelineWrapper.cpp
 *
 *  Created on: 29.06.2016
 *      Author: tly
 */

#include "../Utility/Utils.h"
#include "PipelineWrapper.h"

std::string PipelineWrapper::paramsToJson(const std::map<std::string,std::list<std::string>>& paramsToSave) {
	std::stringstream str;

	bool firstPack = true;
	str << "{\n";
	for(auto& pack : paramsToSave){
		if(!firstPack){
			str << ",\n";
		}else{
			firstPack = false;
		}
		str << "\t\"" << pack.first << "\"" << " : {\n";
		ParamPack& p = this->getParamPack(pack.first);

		bool firstParam = true;
		for(auto& paramName : pack.second){
			if(!firstParam){
				str << ",\n";
			}else{
				firstParam = false;
			}
			Parameter& param = p.getParam(paramName);
			str << "\t\t\"" << param.name << "\"" << " : " << quote(param.getValueAsString());
		}
		str << "\n\t}";
	}
	str << "\n}";
	return str.str();
}

void PipelineWrapper::paramsFromJson(const std::string& jsonObj){
	ParseTree obj = JsonParser::parseJson(jsonObj);
	_log("[info] json: " << printTree(obj));

	assertOrThrow(obj.elementName == "object");
	for(auto& child : obj.children){
		assertOrThrow(child.elementName == "pair" && child.children.back().elementName == "object");
		std::string paramPackName = unquote(child.children.front().children.front().elementName);
		ParamPack& pack = this->getParamPack(paramPackName);
		auto& object = child.children.back();

		for(auto& param : object.children){
			std::string name = unquote(param.children.front().children.front().elementName);
			if(pack.hasParam(name)){
				auto& value = param.children.back();
				assertOrThrow(value.elementName == "string");
				assertOrThrow(value.elementName == "string");
				if(!pack.getParam(name).setValueFromString(unquote(value.children.front().elementName))){
					throw std::runtime_error("invalid value for parameter " + name);
				}
			}else{
				throw std::runtime_error("could not find parameter with name " + name + " inside pack with name " + paramPackName);
			}
		}
	}
}



