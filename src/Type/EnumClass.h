#pragma once
#include "../Utility/VariadicUtils.h"
#include "TypeHelper.h"
#include <vector>
#include <stdexcept>

/*
 * EnumClass.h
 *
 *  Created on: 30.05.2016
 *      Author: tly
 */

template<typename... Strings> struct StringEnum{
	static_assert(sizeof...(Strings) != 0, "StringEnum must have at least 1 element.");

	StringEnum():index(0){}

	StringEnum(const char* s):index(-1){
		if(!this->setString(s)){
			throw std::runtime_error("invalid construction of StringEnum");
		}
	}

	bool setString(const char* s){
		int index = -1;

		int i = 0;
		variadicForEach(
			index == -1 && Strings::toString() == s ? index = i : ++i
		);
		if(index != -1){
			this->index = index;
			return true;
		}else{
			return false;
		}
	}

	std::string getString() const {
		return getPossibleStrings()[index];
	}

	int getIndex() const {
		return index;
	}

	void setIndex(int index){
		this->index = index;
	}

	std::vector<std::string> getPossibleStrings() const {
		static std::string strings[] = {
			Strings::toString()...
		};
		return std::vector<std::string>(strings,strings+sizeof...(Strings));
	}

	bool operator==(const StringEnum<Strings...>& s) const {
		return this->index == s.index;
	}
protected:
	int index;
};

template<typename... Strings> StringEnum<Strings...> makeStringEnum(Strings...){
	return {};
}

#define EnumClass(Name,...) \
using Name = decltype(makeStringEnum(__VA_ARGS__)); \
template<> inline Name fromString<Name>(const std::string& s){ return s.c_str(); } \
template<> inline std::string toString<Name>(const Name& s){ return s.getString(); } \


