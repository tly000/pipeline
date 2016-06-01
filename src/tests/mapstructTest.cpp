/*
 * mapstructTest.cpp
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

#include "../Utility/Utils.h"
#include "../Utility/VariadicUtils.h"
#include "../Type/StringConstantMap.h"
#include "../Type/EnumClass.h"
#include <iostream>

EnumClass(TestEnum,
	"A"_c,
	"B"_c
);

template<typename S,typename... Strings> int indexOf(S,Strings...){
	return IndexOf<S,Strings...>::value;
}

symbolexport void mapstructTest(){
	StringConstantMap<
		KV("element1", int),
		KV("element2", float),
		KV("test", std::string)
	> testMap;
	testMap.at("element1"_c).value = 77;

	testMap.forEach([](auto& v){
		std::cout << v.key() << std::endl;
	});

	TestEnum x = "A";
	x = "B";

	std::cout << x.getString() << std::endl;
}



