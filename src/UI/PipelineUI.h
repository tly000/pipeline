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

#include <iostream>
#include "../UI/PipelineParameterBox.h"
#include "MandelbrotImageView.h"
#include "Platform.h"

struct MainWindow : Gtk::Window{
	MainWindow();

	AbstractPlatform* getSelectedPlatform(){
		return selectedPlatform;
	}

	void calculateNow();
protected:
	void setSensitive(bool s);
	Gtk::HeaderBar header;
	Gtk::Box verticalBox{Gtk::ORIENTATION_VERTICAL};
	Gtk::Paned mainView;
	MandelbrotImageView imageView;

	Gtk::Button calculateButton{"calculate"};
	Gtk::MenuButton saveButton;
	Gtk::ComboBoxText platformBox;
	Gtk::ComboBoxText typeBox;

	Gtk::ScrolledWindow parameterBox;
	sigc::connection calcbuttonConnection;

	std::map<std::string,std::unique_ptr<AbstractPlatform>> platformMap;

	AbstractPlatform* selectedPlatform = nullptr;

	void loadPlatform();
};



