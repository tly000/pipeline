#pragma once
#include <string>
#include <array>

/*
 * MapStruct.h
 *
 *  Created on: 31.05.2016
 *      Author: tly
 */
namespace detail {
    template<size_t N> constexpr std::array<char, N> make_array(const char (&str)[N]) {
        std::array<char, N> arr;
        std::copy_n(str, N, arr.data());
        return arr;
    }
}

template<auto Str> struct StringConstant{
    static std::string toString() {
        return Str.data();
    }
};

template<typename A, typename B>
struct KeyValuePair {
    B value;

    std::string key() const { return A::toString(); }
};

#define STR_CONST_TYPE(s) StringConstant<detail::make_array(s)>
#define _C(s) STR_CONST_TYPE(s)()

#define KV(k, ...) KeyValuePair<STR_CONST_TYPE(k), __VA_ARGS__>
