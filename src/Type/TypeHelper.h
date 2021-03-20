#pragma once

/*
 * TypeHelper.h
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */
#include "../../kernels/Type/Fixed16.h"
#include "../../kernels/Type/Fixed4.h"
#include "../../kernels/Type/Fixed8.h"
#include "../../kernels/Type/Q16_16.h"
#include "../../kernels/Type/Q32_32.h"
#include "../../kernels/Type/double.h"
#include "../../kernels/Type/float.h"
#include "../../kernels/Type/longdouble.h"
#include "../../kernels/Type/qf128.h"
#include "../Platform/Factory.h"
#include <string>

template<typename T> T fromFloatToType(float);

template<> float fromFloatToType<float>(float);
template<> double fromFloatToType<double>(float);
template<> Q16_16 fromFloatToType<Q16_16>(float);
template<> Q32_32 fromFloatToType<Q32_32>(float);
template<> Fixed4 fromFloatToType<Fixed4>(float);
template<> Fixed8 fromFloatToType<Fixed8>(float);
template<> Fixed16 fromFloatToType<Fixed16>(float);
template<> longdouble fromFloatToType<longdouble>(float);
template<> qf128 fromFloatToType<qf128>(float);

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

template<> longdouble fromString<longdouble>(const std::string&);

template<> qf128 fromString<qf128>(const std::string&);

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

template<> std::string toString<longdouble>(const longdouble&);

template<> std::string toString<qf128>(const qf128&);

#ifdef __GNUC__
#include "../Kernel/Type/float128.h"

template<> float128 fromFloatToType<float128>(float);
template<> float128 fromString<float128>(const std::string&);
template<> std::string toString<float128>(const float128&);

#endif

template<> std::shared_ptr<Factory> fromString<std::shared_ptr<Factory>>(const std::string&);
template<> std::string toString<std::shared_ptr<Factory>>(const std::shared_ptr<Factory>&);