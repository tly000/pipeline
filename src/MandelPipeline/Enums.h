#pragma once
#include "../Type/EnumClass.h"

/*
 * Enums.h
 *
 *  Created on: 01.06.2016
 *      Author: tly
 */

EnumClass(MultisamplingPattern,
          _C("UNIFORM_GRID"),
          _C("JITTERING")
);

EnumClass(CalculationMethod,
          _C("normal"),
          _C("successive refinement"),
          _C("Mariani-Silver"),
          _C("grid"),
          _C("successive iteration")
);


