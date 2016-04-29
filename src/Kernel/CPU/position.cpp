#include "position.h"

/*
 * position.cpp
 *
 *  Created on: Apr 12, 2016
 *      Author: tly
 */

void positionKernel(const Range& globalID,const Range& localID,CPUImage<Complex>& image){
	int w = image.getWidth();
	int h = image.getHeight();
	image.at(globalID.x,globalID.y) = Complex{
		floatToType(3 * (float(globalID.x) - w/2) / w),
		floatToType(3 * (float(globalID.y) - h/2) / h)
	};
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



