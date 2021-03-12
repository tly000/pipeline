/*
 * ExternTemplates1.cpp
 *
 *  Created on: 29.06.2016
 *      Author: tly
 */

#include "ExternTemplates.h"

template struct MandelPipeline<CLFactory,float>;
template struct MandelPipeline<CLFactory,double>;
//template struct MandelPipeline<CLFactory,float128>;
template struct MandelPipeline<CLFactory,Fixed4>;
template struct MandelPipeline<CLFactory,Fixed8>;
//template struct MandelPipeline<CLFactory,Fixed16>;


