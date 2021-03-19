#include "CLFactory.h"
#include "../../Utility/Utils.h"
#include "CLBuffer.h"
#include "CLKernel.h"
#include "CLImage.h"

/*
 * CLFactory.cpp
 *
 *  Created on: 16.04.2016
 *      Author: tly
 */

void CLFactory::initOpenCL() {
    try {
        std::vector<cl::Platform> platforms;
        cl::Platform::get(&platforms);

        for (auto &platform : platforms) {
            std::vector<cl::Device> devs;
            platform.getDevices(CL_DEVICE_TYPE_ALL, &devs);
            for (auto &dev : devs) {
                if (dev.getInfo<CL_DEVICE_AVAILABLE>()) {
                    //try to make a context.
                    try {
                        cl::Context context(dev);
                        devices.push_back(dev);
                    } catch (...) {
                        //this device doesnt work.
                    }
                }
            }
        }
    } catch (...) {
        //opencl doesnt work at all...
    }
    openCLInitialized = true;
}

bool CLFactory::openCLInitialized = false;
std::vector<cl::Device> CLFactory::devices{};

CLFactory::CLFactory(uint32_t deviceID) {
    if (!openCLInitialized) {
        initOpenCL();
    }
    if (deviceID == uint32_t(-1)) {
        for (auto &dev : devices) {
            try {
                this->context = cl::Context(dev);
                this->queue = cl::CommandQueue(this->context);
                this->device = dev;
                break;
            } catch (...) {
                this->context = nullptr;
                this->queue = nullptr;
                this->device = nullptr;
            }
        }
        if (!this->device()) {
            throw std::runtime_error("couldnt find any working OpenCL device.");
        }
    } else if (devices.size() > deviceID) {
        this->device = devices.at(deviceID);
        this->context = cl::Context(this->device);
        this->queue = cl::CommandQueue(this->context);
    } else {
        throw std::runtime_error("no OpenCL device with ID " + std::to_string(deviceID) + " found.");
    }
}

std::shared_ptr<RawKernel> CLFactory::createKernel(const std::string &progName, const std::string &kernelName, const std::string &compilerParams) {
    std::string filePath = "kernels/CL/" + progName + ".cl";
    std::string preprocessedFilePath = "temp.cl";
    auto result = systemCommand(
        "cpp -undef -P -D__OPENCL_VERSION__ " + compilerParams +
        " \"" + filePath + "\" " + preprocessedFilePath);
    if (result.first) {
        throw std::runtime_error("error preprocessing " + filePath + ": " + result.second);
    }
    std::string source = fileToString(preprocessedFilePath);
    cl::Program program(context, source);
    try {
        //program.build("-cl-mad-enable -cl-fast-relaxed-math");
        program.build();
        _log("[info] success building program \"" << progName << "\".");
    } catch (const std::exception &err) {
        throw std::runtime_error(
            "build error for program \"" + progName + "\":\n" +
            program.getBuildInfo<CL_PROGRAM_BUILD_LOG>(this->device));
    }
    return std::make_shared<CLKernel>(this->queue, cl::Kernel(program, kernelName.c_str()));
}

std::string CLFactory::getDeviceName() const {
    std::string devName = device.getInfo<CL_DEVICE_NAME>();
    devName = devName.substr(0, devName.size() - 1);
    devName = devName.substr(devName.find_first_not_of(" \t\n\r"));
    return devName + " " + device.getInfo<CL_DEVICE_VERSION>().substr(0, sizeof("OpenCL X.X") - 1);
}

uint32_t CLFactory::getNumberOfDevices() {
    if(!openCLInitialized){
        initOpenCL();
    }
    return devices.size();
}

std::shared_ptr<RawBuffer> CLFactory::createBuffer(std::size_t elemCount, std::size_t elemSize) {
    return std::make_shared<CLBuffer>(context, queue, CL_MEM_READ_WRITE, elemCount, elemSize);
}

std::shared_ptr<RawImage> CLFactory::createImage(std::size_t width, std::size_t height, std::size_t elemSize) {
    return std::make_shared<CLImage>(context, queue, CL_MEM_READ_WRITE, width, height, elemSize);
}
