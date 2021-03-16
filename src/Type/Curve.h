#pragma once
#include <vector>
#include "TypeHelper.h"
#include "../../kernels/Type/CurveSegment.h"

/*
 * Curve.h
 *
 *  Created on: 15.06.2016
 *      Author: tly
 */

using Curve = std::vector<CurveSegment>;

template<> std::string toString<Curve>(const Curve& c);
template<> Curve fromString<Curve>(const std::string& s);

inline bool operator==(const CurveSegment& a,const CurveSegment& b){
	return std::memcmp(&a,&b,sizeof(a)) == 0;
}

inline bool operator!=(const CurveSegment& a,const CurveSegment& b){
	return !(a == b);
}
