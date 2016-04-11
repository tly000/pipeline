#pragma once

/*
 * Kernel.h
 *
 *  Created on: 11.04.2016
 *      Author: tly
 */

template<typename... Inputs>
struct Kernel{
	virtual void run(Inputs&...) = 0;

	virtual ~Kernel() = default;
};


