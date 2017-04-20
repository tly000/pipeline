/*
 * Platform.cpp
 *
 *  Created on: 02.06.2016
 *      Author: tly
 */

#include "Platform.h"
#include "../Type/TypeHelper.h"

template struct Platform<CLFactory,float>;
template struct Platform<CLFactory,double>;
template struct Platform<CLFactory,Fixed4>;
template struct Platform<CLFactory,Fixed8>;
template struct Platform<CLFactory,quadfloat>;

