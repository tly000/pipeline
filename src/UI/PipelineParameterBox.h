#pragma once
#include <gtkmm.h>
#include "../MandelPipeline/PipelineWrapper.h"

/*
 * PipelineParameterToolbar.h
 *
 *  Created on: 03.05.2016
 *      Author: tly
 */

struct PipelineParameterBox : Gtk::Box{
	PipelineParameterBox(PipelineWrapper& pipeline);
};


