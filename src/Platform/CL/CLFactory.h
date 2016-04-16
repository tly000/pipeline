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
inline const char* clewErrorString(cl_int error)
{
    static const char* strings[] =
    {
        // Error Codes
          "CL_SUCCESS"                                  //   0
        , "CL_DEVICE_NOT_FOUND"                         //  -1
        , "CL_DEVICE_NOT_AVAILABLE"                     //  -2
        , "CL_COMPILER_NOT_AVAILABLE"                   //  -3
        , "CL_MEM_OBJECT_ALLOCATION_FAILURE"            //  -4
        , "CL_OUT_OF_RESOURCES"                         //  -5
        , "CL_OUT_OF_HOST_MEMORY"                       //  -6
        , "CL_PROFILING_INFO_NOT_AVAILABLE"             //  -7
        , "CL_MEM_COPY_OVERLAP"                         //  -8
        , "CL_IMAGE_FORMAT_MISMATCH"                    //  -9
        , "CL_IMAGE_FORMAT_NOT_SUPPORTED"               //  -10
        , "CL_BUILD_PROGRAM_FAILURE"                    //  -11
        , "CL_MAP_FAILURE"                              //  -12

        , ""    //  -13
        , ""    //  -14
        , ""    //  -15
        , ""    //  -16
        , ""    //  -17
        , ""    //  -18
        , ""    //  -19

        , ""    //  -20
        , ""    //  -21
        , ""    //  -22
        , ""    //  -23
        , ""    //  -24
        , ""    //  -25
        , ""    //  -26
        , ""    //  -27
        , ""    //  -28
        , ""    //  -29

        , "CL_INVALID_VALUE"                            //  -30
        , "CL_INVALID_DEVICE_TYPE"                      //  -31
        , "CL_INVALID_PLATFORM"                         //  -32
        , "CL_INVALID_DEVICE"                           //  -33
        , "CL_INVALID_CONTEXT"                          //  -34
        , "CL_INVALID_QUEUE_PROPERTIES"                 //  -35
        , "CL_INVALID_COMMAND_QUEUE"                    //  -36
        , "CL_INVALID_HOST_PTR"                         //  -37
        , "CL_INVALID_MEM_OBJECT"                       //  -38
        , "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR"          //  -39
        , "CL_INVALID_IMAGE_SIZE"                       //  -40
        , "CL_INVALID_SAMPLER"                          //  -41
        , "CL_INVALID_BINARY"                           //  -42
        , "CL_INVALID_BUILD_OPTIONS"                    //  -43
        , "CL_INVALID_PROGRAM"                          //  -44
        , "CL_INVALID_PROGRAM_EXECUTABLE"               //  -45
        , "CL_INVALID_KERNEL_NAME"                      //  -46
        , "CL_INVALID_KERNEL_DEFINITION"                //  -47
        , "CL_INVALID_KERNEL"                           //  -48
        , "CL_INVALID_ARG_INDEX"                        //  -49
        , "CL_INVALID_ARG_VALUE"                        //  -50
        , "CL_INVALID_ARG_SIZE"                         //  -51
        , "CL_INVALID_KERNEL_ARGS"                      //  -52
        , "CL_INVALID_WORK_DIMENSION"                   //  -53
        , "CL_INVALID_WORK_GROUP_SIZE"                  //  -54
        , "CL_INVALID_WORK_ITEM_SIZE"                   //  -55
        , "CL_INVALID_GLOBAL_OFFSET"                    //  -56
        , "CL_INVALID_EVENT_WAIT_LIST"                  //  -57
        , "CL_INVALID_EVENT"                            //  -58
        , "CL_INVALID_OPERATION"                        //  -59
        , "CL_INVALID_GL_OBJECT"                        //  -60
        , "CL_INVALID_BUFFER_SIZE"                      //  -61
        , "CL_INVALID_MIP_LEVEL"                        //  -62
        , "CL_INVALID_GLOBAL_WORK_SIZE"                 //  -63
        , "CL_UNKNOWN_ERROR_CODE"
    };

    if (error >= -63 && error <= 0)
         return strings[-error];
    else
         return strings[64];
}

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
