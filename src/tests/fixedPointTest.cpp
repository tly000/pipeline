/*
 * fixedPointTest.cpp
 *
 *  Created on: 22.04.2016
 *      Author: tly
 */

#include <iostream>

#define Type Fixed4
#include "../Kernel/Type/Complex.h"

int main(){
	Type c = fromfloat(0.9);
	Type z = fromfloat(0.9995);

	for(int i = 0; i < 25; i++){
		z = tsub(z,c);
		std::cout << tofloat(z) << std::endl;
	}
}


