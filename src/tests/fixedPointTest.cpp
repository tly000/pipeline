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
	for(int i = 0; i < 300; i++){
		float fA = rand() / float(RAND_MAX) * 500;
		float fB = rand() / float(RAND_MAX) * 500;

		Fixed a = fromfloat(fA);
		Fixed b = fromfloat(fB);

		bool cmpGT[] = {
			a.words[0] > b.words[0],
			a.words[1] > b.words[1],
			a.words[2] > b.words[2],
			a.words[3] > b.words[3]
		};
		bool cmpLT[] = {
			a.words[0] < b.words[0],
			a.words[1] < b.words[1],
			a.words[2] < b.words[2],
			a.words[3] < b.words[3]
		};
		int cmp[] = {
			cmpLT[0] - cmpGT[0],
			cmpLT[1] - cmpGT[1],
			cmpLT[2] - cmpGT[2],
			cmpLT[3] - cmpGT[3]
		};

		int count = cmp[0] * 8 + cmp[1] * 4 + cmp[2] * 2 + cmp[3];

		if(count > 0){
			std::cout << fA << " less than " << fB << " : " << (fA < fB) << ", " << fixedCompareLessThan(a,b) << std::endl;
		}else if(count < 0){
			std::cout << fA << " greater than " << fB << " : " << (fA > fB) << std::endl;
		}else if(count == 0){
			std::cout << fA << " equals " << fB << " : " << (fA == fB) << std::endl;
		}
	}

}


