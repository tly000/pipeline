#include "Utils.h"
#include <fstream>
#include <memory>

/*
 * Utils.cpp
 *
 *  Created on: 29.03.2016
 *      Author: tly
 */

std::string fileToString(const std::string& fileName){
	std::ifstream file(fileName,std::ios::binary);
	if(file){
		auto start = file.tellg();
		file.seekg(0,std::ios::end);
		auto fileSize = file.tellg() - start;
		file.seekg(0,std::ios::beg);

		auto array = std::make_unique<char[]>(fileSize);
		file.read(array.get(),fileSize);
		return std::string(array.get(),array.get()+fileSize);
	} else {
		throw std::runtime_error("file "+ fileName + " could not be opened.");
	}
}


