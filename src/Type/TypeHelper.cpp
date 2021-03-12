#include "TypeHelper.h"
#include <stdexcept>
#include <sstream>
#include <locale>
#include <iomanip>
#include <limits>

/*
 * TypeHelper.cpp
 *
 *  Created on: 05.05.2016
 *      Author: tly
 */

#define fromFloatSpecialization(T) template<> T fromFloatToType(float a){ return _concat(floatTo,T)(a); }

fromFloatSpecialization(float);
fromFloatSpecialization(double);
fromFloatSpecialization(Q16_16);
fromFloatSpecialization(Q32_32);
fromFloatSpecialization(Fixed4);
fromFloatSpecialization(Fixed8);
fromFloatSpecialization(Fixed16);
fromFloatSpecialization(longdouble);
fromFloatSpecialization(qf128);

static bool set_locale() {
    std::setlocale(LC_NUMERIC, "en-US");
    return true;
}

template<> float fromString<float>(const std::string& s){
    static bool l = set_locale();
	char* pos = nullptr;
	float val = std::strtof(s.c_str(),&pos);
	return pos == s.c_str() + s.size() ? val : throw std::runtime_error("invalid conversion");
}

template<> int fromString<int>(const std::string& s){
    static bool l = set_locale();
	char* pos = nullptr;
	int val = std::strtol(s.c_str(),&pos,10);
	return pos == s.c_str() + s.size() ? val : throw std::runtime_error("invalid conversion");
}

template<> uint32_t fromString<uint32_t>(const std::string& s){
    static bool l = set_locale();
	char* pos = nullptr;
	uint32_t val = std::strtoul(s.c_str(),&pos,10);
	return pos == s.c_str() + s.size() ? val : throw std::runtime_error("invalid conversion");
}

template<> double fromString<double>(const std::string& s){
    static bool l = set_locale();
	char* pos = nullptr;
	double val = std::strtod(s.c_str(),&pos);
	return pos == s.c_str() + s.size() ? val : throw std::runtime_error("invalid conversion");
}

template<> std::string fromString<std::string>(const std::string& s){
	return s;
}

template<typename FixedType> void fromStringImpl(std::string s,FixedType& f){
	int N = sizeof(f.words)/sizeof(uint32_t);
	for(int i = 0; i < N; i++){
		f.words[i] = 0;
	}

	bool sign = s[0] == '-';
	if(s[0] == '-' || s[0] == '+'){
		s = s.substr(1);
	}
	auto pointPos = s.find('.');
	//									   ____
	//approximate 0.1 in FixedType, its 0.00011.
	FixedType invertedBase{};
	invertedBase.sign = 0;
	invertedBase.words[0] = 0;
	invertedBase.words[1] = 0b00011001100110011001100110011001;
	for(int i = 2; i < N-1; i++){
		invertedBase.words[i] = 0b10011001100110011001100110011001;
	}
	invertedBase.words[N-1] = 0b10011001100110011001100110011010; //<-- round up,

	//find fractional part
	if(pointPos != s.npos){
		for(uint32_t i = s.size()-1; i > pointPos; i--){
			char c = s.at(i);
			if(isdigit(c)){
				f.words[0] += c - '0';
				f = tmul(f,invertedBase);
			}else if(c == ' '){
				continue; //just ignore
			}else{
				throw std::runtime_error("number format wrong");
			}
		}
	}

	//find integer part
	uint32_t intPart = 0;
	pointPos = pointPos == s.npos ? s.size() : pointPos;
	for(uint32_t i = 0; i < pointPos; i++){
		char c = s.at(i);
		if(isdigit(c)){
			intPart *= 10;
			intPart += c - '0';
		}else if(c == ' '){
			continue; //just ignore
		}else{
			throw std::runtime_error("number format wrong");
		}
	}
	f.words[0] = intPart;
	f.sign = sign;
}

template<> Fixed4 fromString<Fixed4>(const std::string& s){
	Fixed4 result;
	fromStringImpl(s,result);
	return result;
}

template<> Fixed8 fromString<Fixed8>(const std::string& s){
	Fixed8 result;
	fromStringImpl(s,result);
	return result;
}

template<> Fixed16 fromString<Fixed16>(const std::string& s){
	Fixed16 result;
	fromStringImpl(s,result);
	return result;
}

template<> std::string toString<float>(const float& s){
	static std::locale l = std::locale::global(std::locale());
	std::stringstream ss;
	ss.precision(std::numeric_limits<float>::max_digits10);
	ss << std::fixed << s;
	return ss.str();
}

template<> std::string toString<int>(const int& s){
	return std::to_string(s);
}

template<> std::string toString<uint32_t>(const uint32_t& s){
	return std::to_string(s);
}

template<> std::string toString<double>(const double& s){
	static std::locale l = std::locale::global(std::locale());
	std::stringstream ss;
	ss.precision(std::numeric_limits<double>::max_digits10);
	ss << std::fixed << s;
	return ss.str();
}

template<> std::string toString<std::string>(const std::string& s){
	return s;
}

template<typename FixedType> std::string toStringImpl(FixedType f){
	uint32_t N = sizeof(f.words)/sizeof(uint32_t);
	uint32_t numOfFractionDigits = ((N-1) * 32)/3.32; // 3.32 = log(10)/log(2)

	std::string s = (f.sign? "-" : "") + std::to_string(f.words[0]);

	f.words[0] = 0;

	FixedType ten{
		10,0
	};

	std::stringstream str;
	//fractional part

	int nonZeroIndex = 0;
	for(uint32_t i = 0; i < numOfFractionDigits; i++){
		f = tmul(f,ten);
		char newDigit = f.words[0] % 10 + '0';
		str << newDigit;
		if(newDigit != '0'){
			nonZeroIndex = i;
		}
		f.words[0] = 0;
	}

	return s + '.' + str.str().substr(0,nonZeroIndex+1);
}

template<> std::string toString<Fixed4>(const Fixed4& s){
	return toStringImpl(s);
}

template<> std::string toString<Fixed8>(const Fixed8& s){
	return toStringImpl(s);
}

template<> std::string toString<Fixed16>(const Fixed16& s){
	return toStringImpl(s);
}

template<> bool fromString<bool>(const std::string& s) {
	return s == "true" ? true : s == "false" ? false : throw std::runtime_error("invalid string for fromString<bool>");
}

template<> std::string toString<bool>(const bool& b) {
	return b ? "true" : "false";
}

#define _stringize(x) #x
#define stringize(x) _stringize(x)

template<> longdouble fromString<longdouble>(const std::string& s) {
	char* rest = nullptr;
	longdouble f = std::strtold (s.c_str(), &rest);
	if(rest != s.c_str() + s.size()){
		throw std::runtime_error("invalid conversion");
	}
	return f;
}

#include <float.h>

template<> std::string toString<longdouble>(const longdouble& f) {
	char buf[128];
	snprintf(buf,sizeof(buf),"%." stringize(LDBL_DIG) "Lf",f);
	return (const char*)buf;
}

#ifdef QUADMATH_H
fromFloatSpecialization(float128);

template<> float128 fromString<float128>(const std::string& s) {
	char* rest = nullptr;
	float128 f = strtoflt128 (s.c_str(), &rest);
	if(rest != s.c_str() + s.size()){
		throw std::runtime_error("invalid conversion");
	}
	return f;
}



template<> std::string toString<float128>(const float128& f) {
	char buf[128];
	quadmath_snprintf(buf,sizeof(buf),"%." stringize(FLT128_DIG) "Qf",f);
	return (const char*)buf;
}

#endif

template<> qf128 fromString<qf128>(const std::string& s) {
	return qfAssign(fromString<float>(s));
}

template<> std::string toString<qf128>(const qf128& f) {
	return toString<float>(f[0]);
}

#undef stringize
