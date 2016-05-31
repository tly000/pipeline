#include "PipelineParameterBox.h"

/*
 * PipelineParameterBox.cpp
 *
 *  Created on: 03.05.2016
 *      Author: tly
 */

PipelineParameterBox::PipelineParameterBox(PipelineWrapper& pipeline){
	this->set_vexpand(true);

	for(auto& paramPack : pipeline.getParamPacks()){

		std::string name = paramPack.first;
		auto toolItemGroup = Gtk::manage(new Gtk::ToolItemGroup(name));

		auto grid = Gtk::manage(new Gtk::Grid());
		grid->set_column_homogeneous(true);
		grid->set_row_spacing(10);
		grid->set_column_spacing(10);

		int currentYPos = 0;
		for(auto& param : *paramPack.second){
			auto label = Gtk::manage(new Gtk::Label(param->name));
			Gtk::Widget* item = param->createParameterWidget();

			grid->attach(*label,0,currentYPos,1,1);
			grid->attach(*item,1,currentYPos,2,1);
			currentYPos++;
		}

		auto toolItem = Gtk::manage(new Gtk::ToolItem());
		toolItem->set_border_width(10);

		toolItem->add(*grid);
		toolItemGroup->add(*toolItem);

		this->add(*toolItemGroup);
	}
	this->show_all();
}

