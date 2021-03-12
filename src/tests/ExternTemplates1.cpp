/*
 * ExternTemplates1.cpp
 *
 *  Created on: 29.06.2016
 *      Author: tly
 */

#include "ExternTemplates.h"

//template struct MandelPipeline<CPUFactory<true>,float>;
//template struct MandelPipeline<CPUFactory<true>,double>;
//template struct MandelPipeline<CPUFactory<true>,float128>;
//template struct MandelPipeline<CPUFactory<true>,Fixed4>;
template struct MandelPipeline<CPUFactory<true>,Fixed8>;
//template struct MandelPipeline<CPUFactory<true>,Fixed16>;

