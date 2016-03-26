#pragma once
/*
 * VariadicUtils.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

#define variadicForEach(statement) { int dummy[]{ 0,(({statement;}),0)...}; (void)dummy; }

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




