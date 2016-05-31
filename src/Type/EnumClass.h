#pragma once

/*
 * EnumClass.h
 *
 *  Created on: 30.05.2016
 *      Author: tly
 */

template<size_t N, const char* const Strings[N]> struct StringEnum{
	StringEnum(const char* s):index(-1){
		if(!this->setString(s)){
			throw std::runtime_error("invalid construction of StringEnum");
		}
	}

	bool setString(const char* s){
		auto it = std::find(Strings,Strings+N,s);
		if(it != Strings+N){
			this->index = std::distance(Strings,it);
			return true;
		}else{
			return false;
		}
	}

	std::string getString() const {
		return Strings[index];
	}

	int getIndex() const {
		return index;
	}

	void setIndex(int index){
		this->index = index;
	}

	const char* const* getPossibleStrings() const {
		return Strings;
	}
protected:
	int index;
};

#define EnumClass(Name,...) \
constexpr const char* Name##Strings[] = { __VA_ARGS__ }; \
using Name = StringEnum<sizeof(Name##Strings)/sizeof(const char*), (const char* const*)Name##Strings>; \
template<> inline Name fromString<Name>(const std::string& s){ return s.c_str(); } \
template<> inline std::string toString<Name>(const Name& s){ return s.getString(); } \


