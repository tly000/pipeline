#pragma once
#include <string>

/*
 * MapStruct.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

template<char... Chars> struct StringConstant{
	static std::string toString() {
		const char arr[] = {
			Chars...
		};
		return std::string(arr,sizeof(arr));
	}
};

template<typename CharT, CharT... Chars> StringConstant<Chars...> operator"" _c(){
	return {};
}

template<typename A,typename B> struct KeyValuePair{
	B value;

	std::string key() const {
		return A::toString();
	}
};

#define KV(k,...) KeyValuePair<decltype(k##_c), __VA_ARGS__ >
