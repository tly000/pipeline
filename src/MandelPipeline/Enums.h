#pragma once
#include "../Type/EnumClass.h"

/*
 * Enums.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

EnumClass(MultisamplingPattern,
	"UNIFORM_GRID"_c,
	"JITTERING"_c
);

EnumClass(CalculationMethod,
	"normal"_c,
	"successive refinement"_c,
	"Mariani-Silver"_c,
	"grid"_c,
	"successive iteration"_c
);


