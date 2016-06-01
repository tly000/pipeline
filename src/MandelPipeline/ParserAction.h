#pragma once
#include "../Actions/LazyAction.h"
#include "../Parser/Parser.h"
#include <map>

/*
 * ParserAction.h
 *
 *  Created on: 16.05.2016
 *      Author: tly
 */

struct ParserAction : LazyAction<Input(KV("formula",std::string)),Output(KV("processed formula",std::string))>{
	ParserAction(){
		auto identifier = branch("identifier",match("identifier"));
		auto decimal = branch("decimal",match("decimal"));

		auto atomExp = identifier |
					   decimal |
					   form(match("(")) >> ref(exp) >> form(match(")"));
		auto funcExp = branch("func",
			identifier >> form(match("(")) >> ref(exp) >> *(form(match(",")) >> ref(exp)) >> form(match(")"))
		) | atomExp;
		auto powExp = branch("pow",
			funcExp >>
			form(match("^")) >>
			decimal
		) | funcExp;
		auto mulExp = branch("mul",
			powExp >>
			*(
				(match("*") | match("/")) >>
				powExp
			)
		);
		auto addExp = branch("add",
			mulExp >>
			*(
				(match("+") | match("-")) >>
				mulExp
			)
		);
		exp = addExp;
	}
protected:
	std::vector<NamedTokenizerRule> rules{
		{"whitespace",	+(match(' ') | match('\t') | match('\n'))},
		{"decimal",		+digit >> ~(match('.') >> +digit)},
		{"identifier",	+alpha},
		{"+",			match('+')},
		{"-",			match('-')},
		{"*",			match('*')},
		{"/",			match('/')},
		{"(",			match('(')},
		{")",			match(')')},
		{"^",			match('^')},
		{",",			match(',')}
	};

	ParserRule exp = nullptr;

	void executeImpl();

	enum Type{
		REAL,COMPLEX
	};
	struct Expression{
		std::string expressionString;
		Type type;
	};
	Expression buildExpression(const ParseTree&);
	Expression createFunctionExpression(std::string functionName,std::vector<Expression> arguments);

	struct Function{
		std::string name;
		Type returnType;
		std::vector<Type> arguments;
	};

	std::multimap<std::string,Function> functionMap{
		{"add", Function{"tadd",REAL,{REAL,REAL}}},
		{"sub", Function{"tsub",REAL,{REAL,REAL}}},
		{"div", Function{"tdiv",REAL,{REAL,REAL}}},
		{"mul", Function{"tmul",REAL,{REAL,REAL}}},
		{"abs", Function{"tabs",REAL,{REAL}}},
		{"sin", Function{"tsin",REAL,{REAL}}},
		{"cos", Function{"tcos",REAL,{REAL}}},
		{"add", Function{"cadd",COMPLEX,{COMPLEX,COMPLEX}}},
		{"sub", Function{"csub",COMPLEX,{COMPLEX,COMPLEX}}},
		{"div", Function{"cdiv",COMPLEX,{COMPLEX,COMPLEX}}},
		{"mul", Function{"cmul",COMPLEX,{COMPLEX,COMPLEX}}},
		{"abs", Function{"cabs",REAL,{COMPLEX}}},
		{"re", Function{"creal",REAL,{COMPLEX}}},
		{"im", Function{"cimag",REAL,{COMPLEX}}},
		{"pow", Function{"tpow",REAL,{REAL,REAL}}},
		{"conj", Function{"conj",COMPLEX,{COMPLEX}}},
	};
};



