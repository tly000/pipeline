#pragma once

#include "../Type/TypeHelper.h"
#include "../MandelPipeline/MandelPipeline.h"
#include "../Platform/CPU/CPUFactory.h"
#include "../Platform/CL/CLFactory.h"

/*
 * ExternTemplatesCPU.h
 *
 *  Created on: 29.06.2016
 *      Author: tly
 */

extern template struct MandelPipeline<CPUFactory<true>,float>;
extern template struct MandelPipeline<CPUFactory<true>,double>;
extern template struct MandelPipeline<CPUFactory<true>,float128>;
extern template struct MandelPipeline<CPUFactory<true>,Fixed4>;
extern template struct MandelPipeline<CPUFactory<true>,Fixed8>;
extern template struct MandelPipeline<CPUFactory<true>,Fixed16>;

extern template struct MandelPipeline<CPUFactory<false>,float>;
extern template struct MandelPipeline<CPUFactory<false>,double>;
extern template struct MandelPipeline<CPUFactory<false>,float128>;
extern template struct MandelPipeline<CPUFactory<false>,Fixed4>;
extern template struct MandelPipeline<CPUFactory<false>,Fixed8>;
extern template struct MandelPipeline<CPUFactory<false>,Fixed16>;

extern template struct MandelPipeline<CLFactory,float>;
extern template struct MandelPipeline<CLFactory,double>;
extern template struct MandelPipeline<CLFactory,float128>;
extern template struct MandelPipeline<CLFactory,Fixed4>;
extern template struct MandelPipeline<CLFactory,Fixed8>;
extern template struct MandelPipeline<CLFactory,Fixed16>;
