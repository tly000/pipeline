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

EnumClass(NumericType,
          _C("float"),
          _C("double"),
          //_C("Q16_16"),
          //_C("Q32_32"),
          //_C("long double"),
          //_C("float128"),
          _C("qf128"),
          _C("Fixed4"),
          _C("Fixed8"),
          _C("Fixed16")
);


