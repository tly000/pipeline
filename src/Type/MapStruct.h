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

#define KV(k,v) KeyValuePair<decltype(k##_c),v>

template<typename...> struct MapStruct{
	void forEach(...){}
};

template<typename K,typename V, typename...Rest> struct MapStruct<KeyValuePair<K,V>,Rest...> : MapStruct<Rest...>{
	KeyValuePair<K,V> pair;

	template<typename Key> auto& at(const Key& k){
		return MapStruct<Rest...>::template at(k);
	}

	auto& at(const K&){
		return this->pair;
	}

	template<typename F> void forEach(F f){
		f(this->pair);
		MapStruct<Rest...>::forEach(f);
	}
};
