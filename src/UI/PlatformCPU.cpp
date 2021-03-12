#include "Platform.h"
#include "../Type/TypeHelper.h"

/*
 * PlatformCPU.cpp
 *
 *  Created on: 07.06.2016
 *      Author: tly
 */

template struct Platform<CPUFactory<true>,float>;
template struct Platform<CPUFactory<true>,double>;
template struct Platform<CPUFactory<true>,Fixed4>;
template struct Platform<CPUFactory<true>,Fixed8>;


