#include <stdexcept>
#include "../Platform/CPU/CPUFactory.h"
#include "../Platform/CL/CLFactory.h"
#include "../MandelPipeline/MandelPipeline.h"

/*
 * main.cpp
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

symbolexport void pipelineTest(){
	MandelPipeline gpuPipeline;
    gpuPipeline.generalParam.setValue(_C("platform"), std::make_shared<CLFactory>());
	gpuPipeline.run();
}




