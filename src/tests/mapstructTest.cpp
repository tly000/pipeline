/*
 * mapstructTest.cpp
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

#include "../Utility/Utils.h"
#include "../Type/MapStruct.h"
#include <iostream>

symbolexport void mapstructTest(){
	MapStruct<
		KV("element1", int),
		KV("element2", float),
		KV("test", std::string)
	> testMap;
	testMap.at("element1"_c) = 77;

	testMap.forEach([](auto& v){
		std::cout << v.key() << std::endl;
	});
}


