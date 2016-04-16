#include "DynamicLibrary.h"

/*
 * DynamicLibrary.cpp
 *
 *  Created on: 17.04.2016
 *      Author: tly
 */

#if defined(__linux) || defined(__unix)
	#include <dlfcn.h>

	DynamicLibrary::DynamicLibrary(const std::string& fileName)
		:libraryHandle(dlopen(fileName.c_str(),RTLD_NOW),&dlclose){
		if(char* error = dlerror()){
			throw std::runtime_error(std::string("error loading DynamicLibrary: ") + error);
		}
	}

	void* DynamicLibrary::getProcAdress(const std::string& symbolName) const {
		void* funcPtr = dlsym(this->libraryHandle.get(),symbolName.c_str());
		if(char* error = dlerror()){
			throw std::runtime_error(std::string("error on DynamicLibrary::getProcAdress : ") + error);
		}
		return funcPtr;
	}
#elif defined(__APPLE__)
	#error "DynamicLibrary for apple not implemented"
#elif defined(_WIN32)
	#include <windows.h>

	DynamicLibrary::DynamicLibrary(const std::string& fileName)
		:libraryHandle(LoadLibrary(fileName.c_str()),&FreeLibrary){
		if(!libraryHandle.get()){
			throw std::runtime_error(std::string("error loading DynamicLibrary: ") + std::to_string(GetLastError()));
		}
	}

	void* DynamicLibrary::loadSymbol(const std::string& symbolName) const {
		auto funcPtr = GetProcAddress((HMODULE)this->libraryHandle.get(),symbolName.c_str());
		if(!funcPtr){
			throw std::runtime_error(std::string("error on DynamicLibrary::getProcAdress : ") + std::to_string(GetLastError()));
		}
		return reinterpret_cast<void*>(funcPtr);
	}

#else
	#error "os not detected."
#endif
