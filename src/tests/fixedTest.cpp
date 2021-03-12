/*
 * fixedTest.cpp
 *
 *  Created on: 28.07.2016
 *      Author: tly
 */

#include "../Type/TypeHelper.h"
#include <iostream>

//int main(){
//	const Fixed4 zero = fromString<Fixed4>("0");
//	const Fixed4 one = fromString<Fixed4>("1");
//
//	Fixed4 step = {
//		{0,1,0,0},0
//	};
//	Fixed4 step2 = {
//		{0,0,4,0},0
//	};
//
//	Fixed4 sum = zero;
//	for(Fixed4 x = zero; !fixedCompareLessThan(one,x); x = tadd(x,step)){
//		for(Fixed4 y = zero; !fixedCompareLessThan(one,y); y = tadd(y,step)){
//			if(!fixedCompareLessThan(one,tadd(tmul(y,y),tmul(x,x)))){
//				sum = tadd(sum,step2);
//			}
//		}
//	}
//	std::cout << toString(sum) << std::endl;
//}
//
//

