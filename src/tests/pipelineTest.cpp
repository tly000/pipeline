#include "../Platform/CPU/CPUFactory.h"
#include "../Platform/CL/CLFactory.h"
#include "../Kernel/Type/Fixed4.h"
#include "../Kernel/Type/Fixed8.h"
#include "../Kernel/Type/Fixed16.h"
#include "../MandelPipeline/WrappedMandelPipeline.h"

#include <BackTracer.h>

/*
 * main.cpp
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

void pipelinetest(){
	//BackTracer::registerErrorHandler();
	try{
		CLFactory gpu(0);
		CPUFactory cpu;
		CLFactory cpu2(1);

		WrappedMandelPipeline<CLFactory,float> gpuPipeline0(gpu,"float");
		WrappedMandelPipeline<CLFactory,double> gpuPipeline1(gpu,"double");
		WrappedMandelPipeline<CLFactory,Fixed4> gpuPipeline2(gpu,"Fixed4");

		WrappedMandelPipeline<CPUFactory,float> cpuPipeline0(cpu,"float");
		WrappedMandelPipeline<CPUFactory,double> cpuPipeline1(cpu,"double");
		WrappedMandelPipeline<CPUFactory,Fixed4> cpuPipeline2(cpu,"Fixed4");

		WrappedMandelPipeline<CLFactory,float> cpuCLPipeline0(cpu2,"float");
		WrappedMandelPipeline<CLFactory,double> cpuCLPipeline1(cpu2,"double");
		WrappedMandelPipeline<CLFactory,Fixed4> cpuCLPipeline2(cpu2,"Fixed4");

		gpuPipeline0.run();
		cpuPipeline0.run();
		cpuCLPipeline0.run();
	}catch(cl::Error& e){
		std::cout << e.err() << " : " << e.what() << std::endl;
	}
}




