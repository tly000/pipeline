#include "../MandelPipeline/ParserAction.h"

/*
 * parserTest.cpp
 *
 *  Created on: 16.05.2016
 *      Author: tly
 */

symbolexport void parserTest(){
	ParserAction action;
	action.getInput<0>().setDefaultValue("sin(z*z) + c");
	action.run();
}

