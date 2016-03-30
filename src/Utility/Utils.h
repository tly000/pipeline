#pragma once
#include <list>
#include <algorithm>
#include <string>

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

std::string fileToString(const std::string& fileName);
