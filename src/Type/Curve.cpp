#include "Curve.h"
#include "../Utility/Utils.h"
#include <sstream>
#include "../Parser/JsonParser.h"

/*
 * Curve.cpp
 *
 *  Created on: 15.06.2016
 *      Author: tly
 */

template<> std::string toString<Curve>(const Curve& c){
	std::stringstream str;
	str << "[\n";
	for(int i = 0; i < c.size(); i++){
		if(i != 0){
			str << ",\n";
		}
		auto& e = c.at(i);
		str << "{\n\t" << "\"start\": " << "[" << e.start.x << ", " << e.start.y << "],\n";
		str << "\t" << "\"end\": " << "[" << e.end.x << ", " << e.end.y << "],\n";
		str << "\t" << "\"paramA\": " << e.paramA <<",\n";
		str << "\t" << "\"paramB\": " << e.paramB <<",\n";
		str << "\t" << "\"repetition\": " << e.repetition << ",\n";
		str << "\t" << "\"step\": " << e.step << "\n";
		str << "}";
	}
	str << "\n]";
	return str.str();
}

template<> Curve fromString<Curve>(const std::string& s){
	ParseTree tree = JsonParser::parseJson(s);
	Curve c;
	assertOrThrow(tree.elementName == "array");
	for(auto& segment : tree.children){
		CurveSegment s;

		assertOrThrow(segment.elementName == "object");
		assertOrThrow(segment.children.size() == 6);

		auto child = segment.children.begin();

		auto& start = child->children.back();
		assertOrThrow(start.elementName == "array");
		s.start = {
			fromString<float>(start.children.front().children.back().elementName),
			fromString<float>(start.children.back().children.back().elementName),
		};
		++child;

		auto& end = child->children.back();
		assertOrThrow(end.elementName == "array");
		s.end = {
			fromString<float>(end.children.front().children.front().elementName),
			fromString<float>(end.children.back().children.front().elementName),
		};
		++child;

		auto& paramA = child->children.back();
		assertOrThrow(paramA.elementName == "number");
		s.paramA = fromString<float>(paramA.children.front().elementName);
		++child;

		auto& paramB = child->children.back();
		assertOrThrow(paramB.elementName == "number");
		s.paramB = fromString<float>(paramB.children.front().elementName);
		++child;

		auto& repetition = child->children.back();
		assertOrThrow(repetition.elementName == "number");
		s.repetition = fromString<int>(repetition.children.front().elementName);
		++child;

		auto& step = child->children.back();
		assertOrThrow(step.elementName == "number");
		s.step = fromString<int>(step.children.front().elementName);

		c.push_back(s);
	}
	return c;
}
