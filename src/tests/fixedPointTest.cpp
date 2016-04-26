/*
 * fixedPointTest.cpp
 *
 *  Created on: 22.04.2016
 *      Author: tly
 */

#include <iostream>
#include <iomanip>

#define Type Fixed4
#include "../Kernel/Type/Complex.h"

void fixedPointTest(){
	for(float i = -25; i < 25; i++){
		for(float j = -25; j < 25; j++){
			float fA = i * 0.001;
			float fB = j * 0.001;

			Fixed a = fromfloat(fA), b = fromfloat(fB);
			Fixed c = tmul(a,b);

			if(tofloat(c) != (fA*fB)){
				std::cout << fA << " * " << fB << " = " << (fA*fB) << std::endl;
				std::cout << tofloat(a) << " * " << tofloat(b) << " = " << tofloat(c) << std::endl;
				std::cout << std::endl;
			}
		}
	}
}


