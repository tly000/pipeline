#pragma once

/*
 * Multisampling.h
 *
 *  Created on: 10.05.2016
 *      Author: tly
 */

#ifndef MULTISAMPLING_ENABLED
#define MULTISAMPLING_ENABLED false
#endif

#ifndef MULTISAMPLING_SIZE
#define MULTISAMPLING_SIZE 2
#endif

#define UNIFORM_GRID 0
#define JITTERING 1

#ifndef MULTISAMPLING_PATTERN
#define MULTISAMPLING_PATTERN UNIFORM_GRID
#endif


