#pragma once
#include "Utils.h"
#include <memory>

/*
 * DynamicLibrary.h
 *
 *  Created on: 17.04.2016
 *      Author: tly
 */

struct DynamicLibrary{
	DynamicLibrary(const std::string& fileName,bool removeOnDelete);

	void* loadSymbol(const std::string& symbolName) const;
protected:
	std::shared_ptr<void> libraryHandle;
};


