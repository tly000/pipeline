//
// Created by tly on 14.03.2021.
//

#include "CPUFactory.h"
#include "../../Utility/DynamicLibrary.h"
#include "CPUBuffer.h"
#include "CPUImage.h"
#include "CPUKernel.h"

std::shared_ptr<RawBuffer> CPUFactory::createBuffer(std::size_t elemCount, std::size_t elemSize) {
    return std::make_shared<CPUBuffer>(elemCount, elemSize);
}

std::shared_ptr<RawKernel> CPUFactory::createKernel(const std::string &progName, const std::string &kernelName, const std::string &compilerParams) {
    std::string filePath = "./kernels/CPU/" + progName + ".cpp";

    std::string libName = progName;
    int i = 0;
    while(fileExists(libName + ".lib")){
        libName = progName + std::to_string(i);
        i++;
    }
    std::string libPath = libName + ".lib";
    std::pair<int,std::string> output = systemCommand(
        "g++ -O3 -shared -fPIC -std=c++20 " + compilerParams +
        " \"" + filePath + "\""
                           " -o \"" + libPath + "\""
    );
    if(output.first){
        throw std::runtime_error("error compiling program " + filePath + " :\n" + output.second);
    } else {
        _log("[info] successfully compiled " << filePath << ".");
    }

    DynamicLibrary library(getCurrentWorkingDirectory() + "/" + libPath,true);
    throw std::runtime_error("CPUFactory::createKernel TODO");
    // TODO
    /*return std::make_shared<CPUKernel<>>(
        library,
        reinterpret_cast<typename CPUKernel<>::KernelFunc>(library.loadSymbol(kernelName))
    );*/
}

std::shared_ptr<RawImage> CPUFactory::createImage(std::size_t width, std::size_t height, std::size_t elemSize) {
    return std::make_shared<CPUImage>(width, height, elemSize);
}
