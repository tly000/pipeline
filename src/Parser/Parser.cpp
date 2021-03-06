#include "Parser.h"
#include <cctype>
/*
 * Parser.cpp
 *
 *  Created on: 16.05.2016
 *      Author: tly
 */

TokenizerRule match(char c){
	return [c](const char* t){
		return *t == c ? t + 1 : nullptr;
	};
}

TokenizerRule digit = [](const char* t){ return isdigit(*t) ? t+1 : nullptr; };
TokenizerRule alpha = [](const char* t){ return isalpha(*t) ? t+1 : nullptr; };
TokenizerRule nop = [](const char* t){ return t; };

TokenizerRule range(char a,char b){
	return [a,b](const char* t){
		return *t <= b && *t >= a ? t+1 : nullptr;
	};
}

TokenizerRule operator >>(TokenizerRule a, TokenizerRule b) {
	return [a,b](const char* t){
		(t = a(t)) && (t = b(t));
		return t;
	};
}

TokenizerRule operator |(TokenizerRule a, TokenizerRule b) {
	return [a,b](const char* t){
		const char* t1 = nullptr;
		(t1 = a(t)) || (t1 = b(t));
		return t1;
	};
}

TokenizerRule operator*(TokenizerRule rule){
	return ~(rule >> [rule](const char* t){
		return (*rule)(t);
	});
	//return ~(rule >> *rule);
}

TokenizerRule operator+(TokenizerRule rule){
	return rule >> *rule;
}

TokenizerRule operator~(TokenizerRule rule){
	return rule | nop;
}

ParserRule match(std::string t){
	return [t](int pos,const std::vector<Lexeme>& data,ParseTree& tree){
		if(uint32_t(pos) != data.size() && data.at(pos).name == t){
			tree.children.push_back({data.at(pos).token});
			return pos + 1;
		}
		return -1;
	};
}

ParserRule operator*(ParserRule rule){
	return ~(rule >> [rule](int p,const std::vector<Lexeme>& d,ParseTree& t){
		return (*rule)(p,d,t);
	});
}

ParserRule operator+(ParserRule rule){
	return rule >> *rule;
}

ParserRule operator~(ParserRule rule){
	return [rule](int pos,const std::vector<Lexeme>& data,ParseTree& tree){
		int newPos = rule(pos,data,tree);
		return newPos != -1 ? newPos : pos;
	};
}

ParserRule operator >>(ParserRule a, ParserRule b) {
	return [a,b](int pos,const std::vector<Lexeme>& data,ParseTree& tree){
		ParseTree temp;
		int newPos = pos;
		if((newPos = a(newPos,data,temp)) != -1 &&
		   (newPos = b(newPos,data,temp)) != -1	){
			tree.children.insert(tree.children.end(),temp.children.begin(),temp.children.end());
			return newPos;
		}
		return -1;
	};
}

ParserRule operator |(ParserRule a, ParserRule b) {
	return [a,b](int pos,const std::vector<Lexeme>& data,ParseTree& tree){
		int newPos = pos;
		return (newPos = a(pos,data,tree)) != -1 ? newPos : b(pos,data,tree);
	};
}

ParserRule branch(std::string name, ParserRule rule) {
	return [=](int pos,const std::vector<Lexeme>& data,ParseTree& tree){
		ParseTree childTree{name};
		int result = rule(pos,data,childTree);
		if(result != -1){
			tree.children.push_back(childTree);
		}
		return result;
	};
}

ParserRule ref(ParserRule& rule) {
	return [&](int pos,const std::vector<Lexeme>& data,ParseTree& tree){
		return rule(pos,data,tree);
	};
}

ParserRule form(ParserRule rule) {
	return [=](int pos,const std::vector<Lexeme>& data,ParseTree& tree){
		ParseTree temp;
		return rule(pos,data,temp);
	};
}

#include <algorithm>

std::vector<Lexeme> tokenize(const std::vector<NamedTokenizerRule>& rules,const std::vector<std::string>& ignoredRules,const std::string& textString) {
	std::vector<Lexeme> lexemes;
	const char* text = textString.c_str();
	continueOuterLoop: while(*text){
		for(auto& t : rules){
			if(const char* t1 = t.rule(text)){
				if(std::find(ignoredRules.begin(),ignoredRules.end(),t.name) == ignoredRules.end()){
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

#include <cstring>
TokenizerRule text(std::string s) {
	return [s](const char* t){
		return std::strncmp(s.c_str(),t,s.size()) == 0 ? t + s.size() : nullptr;
	};
}

TokenizerRule anyof(std::string s) {
	return [s](const char* t){
		return s.find(*t) == std::string::npos ? nullptr : t+1;
	};
}

TokenizerRule noneof(std::string s) {
	return [s](const char* t){
		return s.find(*t) == std::string::npos ? t+1 : nullptr;
	};
}

std::string printTree(const ParseTree& t){
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
