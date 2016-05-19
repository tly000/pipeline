#include "JsonParser.h"

/*
 * JsonParser.cpp
 *
 *  Created on: 18.05.2016
 *      Author: tly
 */

inline void buildJsonRules(ParserRule& valueRule){
	auto commaSeparated = [](ParserRule r){
		return r >> *(form(match(",")) >> r);
	};

	valueRule = nullptr;
	auto string = branch("string",match("string"));
	auto number = branch("number",match("number"));
	auto object = branch("object",
		form(match("{")) >>
		~commaSeparated(branch("pair",string >> form(match(":")) >> ref(valueRule))) >>
		form(match("}"))
	);
	auto array = branch("array",
		form(match("[")) >>
		~commaSeparated(ref(valueRule)) >>
		form(match("]"))
	);
	valueRule = string | number | object | array | branch("atom",match("atom"));
}

ParseTree JsonParser::parseJson(const std::string& jsonObj) {
	static std::vector<NamedTokenizerRule> tokens{
		{"whitespace",anyof(" \n\t\r")},
		{"{",match('{')},
		{"}",match('}')},
		{"[",match('[')},
		{"]",match(']')},
		{",",match(',')},
		{":",match(':')},
		{"atom",text("true") | text("false") | text("null")},
		{"number",~match('-') >> +digit >> ~(match('.') >> +digit)},
		{"string",match('"') >> *(noneof("\"\\") | (match('\\') >> anyof("\"\\/bfnrtu"))) >> match('"')},
	};
	static ParserRule jsonValue = nullptr;
	static bool ruleInitialized = false;
	if(!ruleInitialized){
		buildJsonRules(jsonValue);
		ruleInitialized = true;
	}

	auto l = tokenize(tokens,{"whitespace"},jsonObj.c_str());
	ParseTree tree{"root"};
	int result = jsonValue(0,l,tree);
	if(uint32_t(result) != l.size()){
		throw std::runtime_error("syntax error somewhere...");
	}
	return tree.children.front();
}


