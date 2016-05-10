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

#define MULTISAMPLING_UNIFORMGRID 0
#define MULTISAMPLING_JITTERGRID 1
#define MULTISAMPLING_GAUSSIAN 2

#ifndef MULTISAMPLING_PATTERN
#define MULTISAMPLING_PATTERN MULTISAMPLING_UNIFORMGRID
#endif


