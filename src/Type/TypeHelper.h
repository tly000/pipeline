#pragma once

/*
 * TypeHelper.h
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */
#include "../Kernel/Type/Q16_16.h"
#include "../Kernel/Type/Q32_32.h"
#include "../Kernel/Type/Fixed4.h"
#include "../Kernel/Type/Fixed8.h"
#include "../Kernel/Type/Fixed16.h"
#include "../Kernel/Type/float.h"
#include "../Kernel/Type/double.h"
#include <string>

template<typename T> T fromFloatToType(float);

template<> float fromFloatToType<float>(float);
template<> double fromFloatToType<double>(float);
template<> Q16_16 fromFloatToType<Q16_16>(float);
template<> Q32_32 fromFloatToType<Q32_32>(float);
template<> Fixed4 fromFloatToType<Fixed4>(float);
template<> Fixed8 fromFloatToType<Fixed8>(float);
template<> Fixed16 fromFloatToType<Fixed16>(float);


template<typename T> T fromString(const std::string&);

template<> float fromString<float>(const std::string&);

template<> int fromString<int>(const std::string&);

template<> uint32_t fromString<uint32_t>(const std::string&);

template<> double fromString<double>(const std::string&);

template<> std::string fromString<std::string>(const std::string&);

template<> Fixed4 fromString<Fixed4>(const std::string&);

template<> Fixed8 fromString<Fixed8>(const std::string&);

template<> Fixed16 fromString<Fixed16>(const std::string&);

template<> bool fromString<bool>(const std::string&);

template<typename T> std::string toString(const T&);

template<> std::string toString<float>(const float&);

template<> std::string toString<int>(const int& s);

template<> std::string toString<uint32_t>(const uint32_t&);

template<> std::string toString<double>(const double&);

template<> std::string toString<std::string>(const std::string&);

template<> std::string toString<Fixed4>(const Fixed4&);

template<> std::string toString<Fixed8>(const Fixed8&);

template<> std::string toString<bool>(const bool&);

template<> std::string toString<Fixed16>(const Fixed16&);
