#pragma once
#include "StringConstant.h"

/*
 * StringConstantMap.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */

template<typename...> struct StringConstantMap{
	void forEach(...){}
};

template<typename K,typename V, typename...Rest> struct StringConstantMap<KeyValuePair<K,V>,Rest...> : StringConstantMap<Rest...>{
	KeyValuePair<K,V> pair;

	template<typename Key> auto& at(const Key& k){
		return StringConstantMap<Rest...>::template at(k);
	}

	auto& at(const K&){
		return this->pair;
	}

	template<typename F> void forEach(F f){
		f(this->pair);
		StringConstantMap<Rest...>::forEach(f);
	}
};


