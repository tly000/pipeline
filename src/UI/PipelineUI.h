#pragma once

/*
 * PipelineUI.h
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */

#include <iostream>
#include <string>
#include <vector>

#include "GtkHeader.h"

#include "../MandelPipeline/MandelPipeline.h"
#include "../UI/PipelineParameterBox.h"
#include "MandelbrotImageView.h"

struct MainWindow : Gtk::Window{
	MainWindow();

	void calculateNow();

    MandelPipeline pipeline;
protected:
	void setSensitive(bool s);
	Gtk::HeaderBar header;
	Gtk::Paned mainView;
	MandelbrotImageView imageView;

	Gtk::Button calculateButton{"calculate"};
	Gtk::MenuButton saveButton;

	Gtk::ScrolledWindow parameterBox;
	sigc::connection calcbuttonConnection;
};



