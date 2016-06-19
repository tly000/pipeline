#pragma once
#include <vector>
#include "../Utility/Utils.h"
#include "Vec.h"
#include "TypeHelper.h"

/*
 * Gradient.h
 *
 *  Created on: 14.06.2016
 *      Author: tly
 */

using Gradient = std::vector<Vec<3,float>>;

template<> std::string toString<Gradient>(const Gradient& g);
template<> Gradient fromString<Gradient>(const std::string& s);

float3 gradientSample(const Gradient& gradient, float t);
