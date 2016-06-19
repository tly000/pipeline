#pragma once
#include <list>
#include <algorithm>
#include <string>
#include <set>
#include <typeinfo>
#include <iostream>
#include <vector>

/*
 * Utils.h
 *
 *  Created on: Mar 26, 2016
 *      Author: tly
 */

template<typename T> bool eraseFromList(std::list<T>& list,const T& el){
	auto it = std::find(list.begin(),list.end(),el);
	if(it != list.end()){
		list.erase(it);
		return true;
	}else{
		return false;
	}
}

std::pair<int,std::string> systemCommand(const std::string& command);

std::string fileToString(const std::string& fileName);
bool fileExists(const std::string& fileName);

std::string loadSourceFileWithHeaders(const std::string& fileName);
std::string loadSourceFileWithHeaders(const std::string& fileName,std::set<std::string>& alreadyLoadedFiles);

std::string demangle(const std::type_info&);
std::string demangle(const char*);

std::string getCurrentWorkingDirectory();

#ifdef DEBUGMESSAGES
#define _logDebug(message) std::cout << "[log] " << demangle(__FUNCTION__) << ":" << __LINE__ << ": " << "[debug] " << message << std::endl
#else
#define _logDebug(message)
#endif

#define _log(message) std::cout << "[log] " << demangle(__FUNCTION__) << ":" << __LINE__ << ": " << message << std::endl

#if defined(__linux) || defined(__unix)
	#define symbolexport extern "C"
#elif defined(__APPLE__)
	#error "Utils for apple not implemented"
#elif defined(_WIN32)
	#define symbolexport extern "C" __declspec(dllexport)
#else
	#error "os not detected."
#endif

#define assertOrThrow(b) { if(!(b)) throw std::runtime_error("assertion failed: " #b); }

template<typename T> bool operator==(const std::vector<T>& a, const std::vector<T>& b){
	if(a.size() == b.size()){
		for(uint32_t i = 0; i < a.size(); i++){
			if(a.at(i) != b.at(i)) return false;
		}
		return true;
	}
	return false;
}

template<typename T> bool operator!=(const std::vector<T>& a, const std::vector<T>& b){
	return !(a == b);
}

std::string quote(const std::string& s);
std::string unquote(const std::string& s);
