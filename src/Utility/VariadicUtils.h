#pragma once
#include <utility>
#include <tuple>

/*
 * VariadicUtils.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

template<size_t N,typename... Types> using NthType = std::tuple_element_t<N,std::tuple<Types...>>;

#define variadicForEach(statement) { int dummy[]{ 0,(statement,0)...}; (void)dummy; }

template<typename Tuple,typename F>
void tupleForEach(Tuple&&, F&&);

template<typename Tuple, typename F,size_t... N>
inline void tupleForEachImpl(Tuple&& t, F&& f,std::index_sequence<N...>) {
	variadicForEach(f(std::get<N>(std::forward<Tuple>(t))))
}

template<typename Tuple, typename F>
inline void tupleForEach(Tuple&& t, F&& f) {
	tupleForEachImpl(
		std::forward<Tuple>(t),
		std::forward<F>(f),
		std::make_index_sequence<std::tuple_size<std::decay_t<Tuple>>::value>{}
	);
}

template<typename T,size_t I> using JustT = T;
template<typename A,typename B> using Just= A;

template<typename... Rest> struct IndexOf{
	enum{ value = -1 };
};

template<typename T,typename... Rest> struct IndexOf<T,T,Rest...>{
	enum{ value = 0 };
};

template<typename T,typename U,typename... Rest> struct IndexOf<T,U,Rest...>{
	enum{ value =
		IndexOf<T,Rest...>::value == -1 ? -1 : 1 + IndexOf<T,Rest...>::value };
};






