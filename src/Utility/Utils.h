#pragma once
#include <list>
#include <algorithm>
#include <string>
#include <set>
#include <typeinfo>
#include <iostream>

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

std::string systemCommand(const std::string& command);

std::string fileToString(const std::string& fileName);

std::string loadSourceFileWithHeaders(const std::string& fileName);
std::string loadSourceFileWithHeaders(const std::string& fileName,std::set<std::string>& alreadyLoadedFiles);

std::string demangle(const std::type_info&);
std::string demangle(const char*);

#define _log(message) std::cout << "[log] " << demangle(__FUNCTION__) << ":" << __LINE__ << ": " << message << std::endl
