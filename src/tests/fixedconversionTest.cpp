/*
 * fixedconversionTest.cpp
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */

#include "../Type/TypeHelper.h"
#include <iostream>

void conversionTest(){
	std::string digits =
			"3.1415926535897932384626433832795028841971693"
			"993751058209749445923078164062862089986280348"
			"253421170679821480865132823066470938446095505"
			"8223172535940812848111745";
	Fixed8 f = fromFloatToType<Fixed8>(std::atof(digits.c_str()));
	Fixed8 f2 = fromString<Fixed8>(digits);
	Fixed16 f3 = fromString<Fixed16>(digits);

	std::cout << toString(f) << std::endl;
	std::cout << toString(f2) << std::endl;
	std::cout << toString(f3) << std::endl;
	std::cout << toString(tofloat(f3)) << std::endl;
}



