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

int main(){
	//BackTracer::registerErrorHandler();
	try{
		//CLFactory gpu(0);
		CPUFactory cpu;
		//CLFactory cpu2(1);

		/*MandelPipelineWrapper<CLFactory,float> gpuPipeline0(gpu,"float");
		MandelPipelineWrapper<CLFactory,double> gpuPipeline1(gpu,"double");
		MandelPipelineWrapper<CLFactory,Fixed4> gpuPipeline2(gpu,"Fixed4");*/

		MandelPipelineWrapper<CPUFactory,float> cpuPipeline0(cpu,"float");
		MandelPipelineWrapper<CPUFactory,double> cpuPipeline1(cpu,"double");
		MandelPipelineWrapper<CPUFactory,Fixed4> cpuPipeline2(cpu,"Fixed4");

		/*MandelPipelineWrapper<CLFactory,float> cpuCLPipeline0(cpu2,"float");
		MandelPipelineWrapper<CLFactory,double> cpuCLPipeline1(cpu2,"double");
		MandelPipelineWrapper<CLFactory,Fixed4> cpuCLPipeline2(cpu2,"Fixed4");*/

		//gpuPipeline0.run();
		cpuPipeline0.run();
		//cpuCLPipeline0.run();
	}catch(cl::Error& e){
		std::cout << e.err() << " : " << e.what() << std::endl;
	}
}




