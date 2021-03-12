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
          _C("A"),
          _C("B")
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
	testMap.at(_C("element1")).value = 77;

	testMap.forEach([](auto& v){
		std::cout << v.key() << std::endl;
	});

	TestEnum x = "A";
	x = "B";

	std::cout << x.getString() << std::endl;
}



