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
#include "../MandelPipeline/MandelPipeline.h"
#include "../Platform/CL/CLFactory.h"
#include "../Platform/CPU/CPUFactory.h"
#include "../UI/PipelineParameterBox.h"
#include "MandelbrotImageView.h"

struct AbstractPlatform{
	virtual PipelineWrapper& getPipeline() = 0;
	virtual PipelineParameterBox& getParameterBox() = 0;
	virtual const CPUImage<unsigned>& getRenderedImage() = 0;

	virtual void scale(float factor) = 0;
	virtual void translate(float x,float y) = 0;
	virtual void rotate(float angle) = 0;
	virtual std::string getName() const = 0;

	virtual std::string paramsToJson(const std::map<std::string,std::list<std::string>>& paramsToSave)= 0;
	virtual void paramsFromJson(const std::string& jsonObj)= 0;

	virtual ~AbstractPlatform() = default;
};

struct MainWindow : Gtk::Window{
	MainWindow();

	AbstractPlatform* getSelectedPlatform(){
		return selectedPlatform;
	}

	void calculateNow();
protected:
	Gtk::HeaderBar header;
	Gtk::Box verticalBox{Gtk::ORIENTATION_VERTICAL};
	Gtk::Paned mainView;
	MandelbrotImageView imageView;

	Gtk::Button calculateButton{"calculate"};
	Gtk::MenuButton saveButton;
	Gtk::ComboBoxText platformBox;
	Gtk::ComboBoxText typeBox;

	Gtk::ScrolledWindow parameterBox;


	std::list<std::unique_ptr<AbstractPlatform>> platforms;
	std::map<std::pair<std::string,std::string>,AbstractPlatform*> platformMap;

	AbstractPlatform* selectedPlatform = nullptr;

	void loadPlatform();
};



