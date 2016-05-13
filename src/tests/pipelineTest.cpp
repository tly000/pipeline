#include "../Platform/CPU/CPUFactory.h"
#include "../Platform/CL/CLFactory.h"
#include "../Kernel/Type/Fixed4.h"
#include "../Kernel/Type/Fixed8.h"
#include "../Kernel/Type/Fixed16.h"
#include <BackTracer.h>
#include "../MandelPipeline/MandelPipelineWrapper.h"

/*
 * main.cpp
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

symbolexport void pipelineTest(){
	BackTracer::registerErrorHandler();
	CLFactory gpu;

	MandelPipelineWrapper<CLFactory,float> gpuPipeline(gpu,"float");
	gpuPipeline.run();
}




