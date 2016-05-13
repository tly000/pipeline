#pragma once
#include "../MandelPipeline/PipelineWrapper.h"
#include "GtkHeader.h"

/*
 * PipelineParameterToolbar.h
 *
 *  Created on: 03.05.2016
 *      Author: tly
 */

struct PipelineParameterBox : Gtk::ToolPalette{
	PipelineParameterBox(PipelineWrapper& pipeline);
};


