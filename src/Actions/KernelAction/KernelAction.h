#pragma once
#include "../../Pipeline/StaticPipelineAction.h"

/*
 * KernelAction.h
 *
 *  Created on: 10.04.2016
 *      Author: tly
 */

template<typename KernelType,typename... Inputs>
struct KernelAction : StaticPipelineAction<Input(KernelType,Inputs...),Output()>{};

