#include "../Platform/CPU/CPUFactory.h"
#include "../Platform/CL/CLFactory.h"
#include "../Kernel/Type/Fixed4.h"
#include "../Kernel/Type/Fixed8.h"
#include "../Kernel/Type/Fixed16.h"
#include "../MandelPipeline/MandelPipeline.h"

/*
 * main.cpp
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

symbolexport void pipelineTest(){
	CLFactory gpu;

	MandelPipeline<CLFactory,float> gpuPipeline(gpu,"float");
	gpuPipeline.run();
}




