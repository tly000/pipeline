#pragma once
#include "CL.h"
#include "CLBuffer.h"
#include "CLImage.h"
#include "CLKernel.h"
#include "../../Utility/Utils.h"

/*
 * CLFactory.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

struct CLFactory{
	CLFactory(uint32_t deviceID = -1){
		static bool openCLInitialized = false;
		if(!openCLInitialized){
			initOpenCL();
			openCLInitialized = true;
		}
		if(deviceID == uint32_t(-1)){
			for(auto& dev : devices){
				try{
					this->context = cl::Context(dev);
					this->queue = cl::CommandQueue(this->context);
					this->device = dev;
					break;
				}catch(cl::Error& e){}
			}
			if(!this->device()){
				throw std::runtime_error("couldnt find any working OpenCL device.");
			}
		}else if(devices.size() > deviceID){
			this->device = devices.at(deviceID);
			this->context = cl::Context(this->device);
			this->queue = cl::CommandQueue(this->context);
		}else{
			throw std::runtime_error("no OpenCL device with ID " + std::to_string(deviceID) + " found.");
		}
	}

	template<typename T> using Image = CLImage<T>;
	template<typename T> using Buffer= CLBuffer<T>;
	template<typename... Inputs> using Kernel = CLKernel<Inputs...>;

	template<typename T> Image<T> createImage(uint32_t width,uint32_t height){
		return CLImage<T>(context,CL_MEM_READ_WRITE,width,height);
	}
	template<typename T> Buffer<T> createBuffer(uint32_t elemCount){
		return CLBuffer<T>(elemCount);
	}
	template<typename... Inputs> Kernel<Inputs...> createKernel(const std::string& progName,const std::string& kernelName){
		std::string source = loadSourceFileWithHeaders("src/Kernel/CL/" + progName + ".cl");
		cl::Program program(context,source);
		try{
			program.build();
			_log("[info] success building program \"" << progName << "\".");
		}catch(const std::exception& err){
			throw std::runtime_error(
				"build error for program \"" + progName + "\":\n" +
				program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->device)
			);
		}
		return CLKernel<Inputs...>(this->queue,cl::Kernel(program,kernelName.c_str()));
	}

	std::string getDeviceName() const {
		return device.getInfo<CL_DEVICE_NAME>();
	}
protected:
	cl::Device device;
	cl::Context context;
	cl::CommandQueue queue;

	static void initOpenCL();
	static std::vector<cl::Device> devices;
};
