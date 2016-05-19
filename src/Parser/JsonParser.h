#pragma once
#include "../Parser/Parser.h"

/*
 * JsonParser.h
 *
 *  Created on: 18.05.2016
 *      Author: tly
 */

struct JsonParser{
	static ParseTree parseJson(const std::string& jsonObj);
};
