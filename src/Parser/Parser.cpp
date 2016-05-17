#include "Parser.h"

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

TokenizerRule operator*(TokenizerRule rule){
	return [rule](const char* t){
		const char* currentPos = t;
		while((currentPos = rule(currentPos))){
			t = currentPos;
		}
		return t;
	};
}

TokenizerRule operator+(TokenizerRule rule){
	return rule >> *rule;
}

TokenizerRule operator~(TokenizerRule rule){
	return [rule](const char* t){
		const char* t1 = rule(t);
		return t1 ? t1 : t;
	};
}

TokenizerRule range(char a,char b){
	return [a,b](const char* t){
		return *t <= b && *t >= a ? t+1 : nullptr;
	};
}

TokenizerRule digit = [](const char* t){ return isdigit(*t) ? t+1 : nullptr; };
TokenizerRule alpha = [](const char* t){ return isalpha(*t) ? t+1 : nullptr; };

TokenizerRule operator >>(TokenizerRule a, TokenizerRule b) {
	return [a,b](const char* t){
		return (t = a(t)) && (t = b(t)) ? t : nullptr;
	};
}

TokenizerRule operator |(TokenizerRule a, TokenizerRule b) {
	return [a,b](const char* t){
		const char* t1 = nullptr;
		return (t1 = a(t)) || (t1 = b(t)) ? t1 : nullptr;
	};
}

ParserRule match(std::string t){
	return [t](int pos,const std::vector<Lexeme>& data,ParseTree& tree){
		if(pos != data.size() && data.at(pos).name == t){
			tree.children.push_back({data.at(pos).token});
			return pos + 1;
		}
		return -1;
	};
}

ParserRule operator*(ParserRule rule){
	return [rule](int pos,const std::vector<Lexeme>& data,ParseTree& tree){
		int currentPos = pos;
		while((currentPos = rule(currentPos,data,tree)) != -1){
			pos = currentPos;
		}
		return pos;
	};
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
