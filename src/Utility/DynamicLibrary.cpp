#include "DynamicLibrary.h"
#include <cstdio>

/*
 * DynamicLibrary.cpp
 *
 *  Created on: 17.04.2016
 *      Author: tly
 */

#if defined(__linux) || defined(__unix)
	#include <dlfcn.h>

	DynamicLibrary::DynamicLibrary(const std::string& fileName,bool removeOnDelete)
		:libraryHandle(dlopen(fileName.c_str(),RTLD_NOW),[=](void* l){ dlclose(l); if(removeOnDelete){ std::remove(fileName.c_str()); }}){
		if(char* error = dlerror()){
			throw std::runtime_error(std::string("error loading DynamicLibrary: ") + error);
		}
	}

	void* DynamicLibrary::loadSymbol(const std::string& symbolName) const {
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

	//http://stackoverflow.com/questions/1387064/how-to-get-the-error-message-from-the-error-code-returned-by-getlasterror
	//Returns the last Win32 error, in string format. Returns an empty string if there is no error.
	static std::string GetLastErrorAsString(){
	    //Get the error message, if any.
	    DWORD errorMessageID = ::GetLastError();
	    if(errorMessageID == 0)
	        return std::string(); //No error message has been recorded

	    LPSTR messageBuffer = nullptr;
	    size_t size = FormatMessageA(
	    	FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
	        NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	    std::string message(messageBuffer, size);

	    //Free the buffer.
	    LocalFree(messageBuffer);

	    return message;
	}

	DynamicLibrary::DynamicLibrary(const std::string& fileName,bool removeOnDelete)
		:libraryHandle(LoadLibrary(fileName.c_str()),[=](void* l){ FreeLibrary((HMODULE)l); if(removeOnDelete){ std::remove(fileName.c_str()); }}){
		if(!libraryHandle.get()){
			throw std::runtime_error(std::string("error loading DynamicLibrary " + fileName + ": ") + GetLastErrorAsString());
		} else {
			_log("[info] successfully loaded " << fileName);
		}
	}

	void* DynamicLibrary::loadSymbol(const std::string& symbolName) const {
		auto funcPtr = GetProcAddress((HMODULE)this->libraryHandle.get(),symbolName.c_str());
		if(!funcPtr){
			throw std::runtime_error(std::string("error on DynamicLibrary::getProcAdress : ") + GetLastErrorAsString());
		}
		return reinterpret_cast<void*>(funcPtr);
	}

#else
	#error "os not detected."
#endif
