#include "Platform.h"
#include "../Type/TypeHelper.h"

/*
 * PlatformCPU.cpp
 *
 *  Created on: 07.06.2016
 *      Author: tly
 */

template struct Platform<CPUFactory,float>;
template struct Platform<CPUFactory,double>;
template struct Platform<CPUFactory,longdouble>;
#ifdef QUADMATH_H
template struct Platform<CPUFactory,float128>;
#endif
template struct Platform<CPUFactory,Fixed4>;
template struct Platform<CPUFactory,Fixed8>;
template struct Platform<CPUFactory,Fixed16>;


