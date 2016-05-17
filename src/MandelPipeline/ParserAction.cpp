#include "ParserAction.h"
#include "../Utility/Utils.h"
/*
 * ParserAction.cpp
 *
 *  Created on: 16.05.2016
 *      Author: tly
 */

std::vector<Lexeme> ParserAction::tokenize(const char* text){
	std::vector<Lexeme> lexemes;
	continueOuterLoop: while(*text){
		for(auto& t : tokens){
			if(const char* t1 = t.rule(text)){
				if(t.name != "whitespace"){
					lexemes.push_back({t.name,std::string(text,uint32_t(t1-text))});
				}
				text = t1;
				goto continueOuterLoop;
			}
		}
		return {};
	}

	return lexemes;
}

inline std::string printTree(const ParseTree& t){
	std::string s = t.elementName;
	if(t.children.size()){
		s += '(';
		for(auto& c : t.children){
			s += printTree(c) + ",";
		}
		s.at(s.size()-1) = ')';
	}
	return s;
}

void ParserAction::executeImpl() {
	std::string text = this->getInput<0>().getValue();
	auto l = this->tokenize(text.c_str());
	ParseTree tree{"root"};
	int result = this->exp(0,l,tree);
	if(uint32_t(result) != l.size()){
		throw std::runtime_error("syntax error somewhere...");
	}
	Expression exp = this->buildExpression(tree);
	_log("[info] " << exp.expressionString);
	this->getOutput<0>().setValue(exp.expressionString);
}

ParserAction::Expression ParserAction::createFunctionExpression(std::string functionName,std::vector<ParserAction::Expression> arguments){
	auto func = this->functionMap.equal_range(functionName);
	if(func.first == this->functionMap.end()){
		throw std::runtime_error("function " + functionName + " could not be found.");
	}
	for(auto it = func.first; it != func.second; ++it){
		auto& argumentTypes = it->second.arguments;

		if(argumentTypes.size() == arguments.size()){
			std::string exp = it->second.name + "(";

			for(uint32_t i = 0; i < arguments.size(); i++){
				auto argType = argumentTypes.at(i);
				auto expType = arguments.at(i).type;
				if(argType == expType){
					exp += arguments.at(i).expressionString;
				}else if(argType == COMPLEX && expType == REAL){
					exp += "(Complex){" + arguments.at(i).expressionString + ", floatToType(0)}";
				}else{
					goto continueOuterLoop;
				}
				exp += ",";
			}

			if(arguments.empty()){
				exp += ")";
			}else{
				exp.back() = ')';
			}

			return Expression{
				exp, it->second.returnType
			};
		}
		continueOuterLoop: ;
	}
	throw std::runtime_error("no suitable function found for '" + functionName + "'.");
}

ParserAction::Expression ParserAction::buildExpression(const ParseTree& tree) {
	if(tree.elementName == "root"){
		return this->buildExpression(tree.children.front());
	}else if(tree.elementName == "add" || tree.elementName == "mul"){
		Expression exp = this->buildExpression(tree.children.front());

		auto currentPos = ++ tree.children.begin();
		while(currentPos != tree.children.end()){
			std::string op = currentPos->elementName;
			Expression right = this->buildExpression(*++currentPos);

			std::string funcName =
				op == "+" ? "add" :
				op == "-" ? "sub" :
				op == "*" ? "mul" :
				op == "/" ? "div" :
				throw std::runtime_error("unknown operator" + op);

			exp = this->createFunctionExpression(funcName,{exp,right});

			++currentPos;
		}
		return exp;
	}else if(tree.elementName == "identifier"){
		std::string name = tree.children.front().elementName;
		if(name == "i"){
			return Expression{ "(Complex){ floatToType(0), floatToType(1) }", COMPLEX };
		}else if(name == "z" || name == "c"){
			return Expression{ name, COMPLEX };
		}else{
			throw std::runtime_error("unknown identifier "+name);
		}
	}else if(tree.elementName == "func"){
		std::string name = tree.children.front().children.front().elementName;

		std::vector<Expression> arguments;
		auto currentPos = ++ tree.children.begin();
		while(currentPos != tree.children.end()){
			arguments.push_back(this->buildExpression(*currentPos));
			++ currentPos;
		}
		return this->createFunctionExpression(name,arguments);
	}else if(tree.elementName == "pow"){
		Expression exp = this->buildExpression(tree.children.front());

		std::string exponent = tree.children.back().children.front().elementName;
		if(exp.type == COMPLEX && exponent.size() == 1 && range('2','9')(exponent.c_str())){
			return Expression{
				"cpow" + exponent + "(" + exp.expressionString + ")",
				COMPLEX
			};
		}else{
			Expression exponentExpression = this->buildExpression(tree.children.back());
			return this->createFunctionExpression("pow",{exp,exponentExpression});
		}
	}if(tree.elementName == "decimal"){
		std::string number = tree.children.front().elementName;
		return Expression{ "floatToType(" + number + ")", REAL };
	}else {
		throw std::runtime_error("unknown element in parse tree: " + tree.elementName);
	}
}
