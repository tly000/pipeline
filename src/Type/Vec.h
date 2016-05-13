#pragma once
#include <type_traits>

/*
 * Vec.h
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

template<unsigned N,typename T> struct Vec{
	T data[N] = {};
	T& operator[](size_t i){
		return data[i];
	}
	const T& operator[](size_t i) const{
		return data[i];
	}

	template<unsigned M,typename U> explicit operator Vec<M,U>(){
		Vec<M,U> result{};
		for(int i = 0; i < M && i < N; i++){
			result[i] = U(data[i]);
		}
		return result;
	}
};

//define some ops

#define binaryOp(op,opEq) \
	template<unsigned N,typename T,typename U> Vec<N,std::common_type_t<T,U>> operator op(const Vec<N,T>& a,const Vec<N,U>& b){ \
		Vec<N,std::common_type_t<T,U>> result{}; \
		for(int i = 0; i < N; i++){ \
			result.data[i] = a.data[i] op b.data[i]; \
		} \
		return result; \
	}\
	template<unsigned N,typename T,typename U> Vec<N,std::common_type_t<T,U>> operator op(const U& a,const Vec<N,T>& b){ \
		Vec<N,std::common_type_t<T,U>> result{}; \
		for(int i = 0; i < N; i++){ \
			result.data[i] = a op b.data[i]; \
		} \
		return result; \
	}\
	template<unsigned N,typename T,typename U> Vec<N,std::common_type_t<T,U>> operator op(const Vec<N,T>& a,const U& b){ \
		Vec<N,std::common_type_t<T,U>> result{}; \
		for(int i = 0; i < N; i++){ \
			result.data[i] = a.data[i] op b; \
		} \
		return result; \
	}\
	template<unsigned N,typename T,typename U> Vec<N,T>& operator opEq(Vec<N,T>& a,const Vec<N,U>& b){ \
		for(int i = 0; i < N; i++){ \
			a.data[i] opEq b.data[i]; \
		} \
		return a; \
	}\
	template<unsigned N,typename T,typename U> Vec<N,T>& operator opEq(Vec<N,T>& a,const U& b){ \
		for(int i = 0; i < N; i++){ \
			a.data[i] opEq b; \
		} \
		return a; \
	}\

binaryOp(+,+=)
binaryOp(-,-=)
binaryOp(/,/=)
binaryOp(*,*=)
binaryOp(%,%=)
binaryOp(>>,>>=)
binaryOp(<<,<<=)
binaryOp(&,&=)
binaryOp(|,|=)
binaryOp(^,^=)

