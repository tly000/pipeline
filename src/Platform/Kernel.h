#pragma once
#include "../Type/Range.h"

/*
 * Kernel.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename... Inputs>
struct Kernel{
	virtual void run(
		const Range& globalOffset,
		const Range& globalSize,
		const Range& localSize,
		Inputs&...
	) = 0;

	virtual ~Kernel() = default;
};


