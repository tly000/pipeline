#include "Gradient.h"
#include "../Utility/Utils.h"
#include "../Parser/JsonParser.h"
#include <sstream>

/*
 * Gradient.cpp
 *
 *  Created on: 14.06.2016
 *      Author: tly
 */

template<> std::string toString<Gradient>(const Gradient& g){
	std::stringstream str;
	str << "[\n";
	if(!g.empty()){
		auto& e = g.front();
		str << "[" << e[0] << ", " << e[1] << ", " << e[2] << "]";
	}
	for(int i = 1; i < g.size(); i++){
		str << ",\n";
		auto& e = g.at(i);
		str << "[" << e[0] << ", " << e[1] << ", " << e[2] << "]";
	}
	str << "\n]";
	return str.str();
}
template<> Gradient fromString<Gradient>(const std::string& s){
	ParseTree tree = JsonParser::parseJson(s);
	Gradient g;
	assertOrThrow(tree.elementName == "array");
	for(auto& color : tree.children){
		assertOrThrow(color.elementName == "array");
		assertOrThrow(color.children.size() == 3);
		Vec<3,float> col;
		int i = 0;
		for(auto& component : color.children){
			assertOrThrow(component.elementName == "number");
			col[i] = fromString<float>(component.children.front().elementName);
			i++;
		}
		g.push_back(col);
	}
	return g;
}

bool operator==(const Gradient& a,const Gradient& b){
	if(a.size() == b.size()){
		for(int i = 0; i < a.size(); i++){
			if(a.at(i) != b.at(i)) return false;
		}
		return true;
	}
	return false;
}



