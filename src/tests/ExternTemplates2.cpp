/*
 * ExternTemplates1.cpp
 *
 *  Created on: 29.06.2016
 *      Author: tly
 */

#include "ExternTemplates.h"

template struct MandelPipeline<CPUFactory<false>,float>;
template struct MandelPipeline<CPUFactory<false>,double>;
//template struct MandelPipeline<CPUFactory<false>,float128>;
template struct MandelPipeline<CPUFactory<false>,Fixed4>;
template struct MandelPipeline<CPUFactory<false>,Fixed8>;
//template struct MandelPipeline<CPUFactory<false>,Fixed16>;


