#include "position.h"
#include <iostream>

/*
 * position.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

void positionKernel(const Range& globalID,const Range& localID,CPUImage<Complex>& image,Type& offsetReal,Type& offsetImag,Type& scale){
	int w = image.getWidth();
	int h = image.getHeight();
	Complex pos = {
		floatToType(2 * (float(globalID.x) - w/2) / w),
		floatToType(2 * (float(globalID.y) - h/2) / h * (float)h/w)
	};
	Complex scaleFactor ={
		scale, floatToType(0)
	};
	Complex offset ={
		offsetReal, offsetImag
	};
	image.at(globalID.x,globalID.y) = cadd(cmul(scaleFactor,pos),offset);
}

#undef Type

#ifdef _WIN32
#include <windows.h>

bool DllMain(
  _In_ HINSTANCE hinstDLL,
  _In_ DWORD     fdwReason,
  _In_ LPVOID    lpvReserved
){
	return true;
}

#endif



