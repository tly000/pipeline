#include <iostream>

/*
 * main.cpp
 *
 *  Created on: 11.05.2016
 *      Author: tly
 */

#ifdef _WIN32
#include <windows.h>
int main2(){
	auto h = GetModuleHandle(nullptr);

	std::cout << "insert function name" << std::endl;
	std::string funcName;
	std::cin >> funcName;

	auto fptr = GetProcAddress(h,funcName.c_str());
	if(fptr){
		reinterpret_cast<void(*)()>(fptr)();
	}else{
		std::cerr << "wrong function name" << std::endl;
	}
}

#endif
