/*
 * performanceTests.cpp
 *
 *  Created on: 29.06.2016
 *      Author: tly
 */

#include "../MandelPipeline/MandelPipeline.h"
#include "../Platform/CL/CLFactory.h"
#include "../Platform/CPU/CPUFactory.h"
#include "../stb_image_write.h"
#include <fstream>

void testPipeline(std::shared_ptr<Factory> f,std::string pipelineName, std::string typeName,std::string test,std::string method,int testCount){
	std::cout << "testing pipeline " << pipelineName << " with type " << typeName << " and method " << method << std::endl;
	MandelPipeline pipeline(f);

	std::string testFile = fileToString("./" + test + ".json");
	pipeline.paramsFromJson(testFile);
	pipeline.calcParams.setValue(_C("calculation method"),method.c_str());

	pipeline.run();

	std::ofstream log(test+".log",std::ios::out | std::ios::app);

	log << pipelineName << "_" << typeName + "_" << test << "_" << method << ", ";
	std::vector<uint64_t> times;
	for(int i = 0; i < testCount; i++){
		pipeline.run();
		log << pipeline.calcAction.getOutput(_C("time")).getValue() << ", ";
		times.push_back(pipeline.calcAction.getOutput(_C("time")).getValue());
	}
	log << std::endl;
	log << std::endl;
	log.close();

	auto& renderedImage = pipeline.reductionAction.getOutput(_C("reducedImage")).getValue();
    std::vector<unsigned> image(renderedImage.getWidth() * renderedImage.getHeight());
    renderedImage.getRawImage()->copyToBuffer(image.data(), sizeof(unsigned) + image.size());

	std::string fileName = pipelineName + "_" + typeName + "_" + test + "_" + method + ".png";
	stbi_write_png(fileName.c_str(),renderedImage.getWidth(),renderedImage.getHeight(),4,image.data(),0);
}

void preftest(){
	auto cpuOpenMP = std::make_shared<CPUFactory>();
    auto gpu = std::make_shared<CLFactory>(1);
    auto cpuOpenCL = std::make_shared<CLFactory>(2);

#define _testPipeline(type,pipeline,test,method,count) testPipeline(pipeline,#pipeline,#type,test,method,count)

#define _testType(type,factory,test,count) \
	for(auto& method : { \
		"normal","successive refinement","Mariani-Silver","successive iteration" \
	}){\
		_testPipeline(type,factory,test,method,count); \
	}

#define _testPlatform(factory,test) \
	_testType(Fixed8,factory,test,3); \
	//_testType(float,factory,test,20); \
	//_testType(double,factory,test,20); \
	//_testType(Fixed4,factory,test,5); \


	_testPlatform(cpuOpenMP,"test4");
	//_testPlatform(cpuNoOpenMP,"test2");
	//_testPlatform(cpuOpenCL,"test4");
}


