#pragma once
#include <functional>
#include <vector>
#include <list>

/*
 * Parser.h
 *
 *  Created on: 16.05.2016
 *      Author: tly
 */

struct Lexeme{
	std::string name,token;
};

struct ParseTree{
	std::string elementName;
	std::list<ParseTree> children;
};

using TokenizerRule = std::function<const char*(const char*)>;
using ParserRule = std::function<int(int pos,const std::vector<Lexeme>& data,ParseTree&)>;

//TokenizerRules
TokenizerRule match(char c);
TokenizerRule text(std::string t);

TokenizerRule operator*(TokenizerRule rule); //rule*
TokenizerRule operator+(TokenizerRule rule); //rule+
TokenizerRule operator~(TokenizerRule rule); //optional, rule?

TokenizerRule range(char a,char b);
TokenizerRule operator>>(TokenizerRule a,TokenizerRule b); //sequence
TokenizerRule operator|(TokenizerRule a,TokenizerRule b);  //alternative

extern TokenizerRule digit;
extern TokenizerRule alpha;

//ParserRules
ParserRule match(std::string t);

ParserRule operator*(ParserRule rule); //rule*
ParserRule operator+(ParserRule rule); //rule+
ParserRule operator~(ParserRule rule); //optional

ParserRule operator>>(ParserRule a,ParserRule b); //sequence
ParserRule operator|(ParserRule a,ParserRule b);  //alternative

ParserRule branch(std::string name,ParserRule rule);
ParserRule ref(ParserRule& rule);
ParserRule form(ParserRule rule);


