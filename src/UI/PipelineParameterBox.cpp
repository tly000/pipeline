#include "PipelineParameterBox.h"

/*
 * PipelineParameterBox.cpp
 *
 *  Created on: 03.05.2016
 *      Author: tly
 */

struct ParameterItem : Gtk::Entry{
	ParameterItem(Parameter& param)
	:param(param){
		this->add_events(Gdk::EventMask::KEY_PRESS_MASK);
		this->set_text(param.getValueAsString());
		this->signal_focus_out_event().connect([this](GdkEventFocus*){
			this->commitChange();
			return false;
		});
		this->signal_key_press_event().connect([this](GdkEventKey* e){
			if(e->keyval == GDK_KEY_Return){
				this->commitChange();
			}
			return false;
		});
		this->param.registerObserver([this](Parameter*){
			this->set_text(this->param.getValueAsString());
		});
	}
protected:
	Parameter& param;

	void commitChange(){
		if(!this->param.setValueFromString(this->get_text().raw())){
			this->set_text(this->param.getValueAsString());
		}
	}
};

PipelineParameterBox::PipelineParameterBox(PipelineWrapper& pipeline)
	:Gtk::Box(Gtk::Orientation::ORIENTATION_VERTICAL){
	std::cout << demangle(typeid(pipeline)) << std::endl;
	auto params = pipeline.getParamPacks();

	auto grid = Gtk::manage(new Gtk::Grid());

	int currentYPos = 0;
	for(auto& paramPack : params){
		std::string name = paramPack.first;
		auto nameLabel = Gtk::manage(new Gtk::Label(name));
		grid->attach(*nameLabel,0,currentYPos,3,1);
		currentYPos++;

		for(auto& param : *paramPack.second){
			auto label = Gtk::manage(new Gtk::Label(param->name));
			auto item = Gtk::manage(
				new ParameterItem(*param)
			);
			grid->attach(*label,0,currentYPos,1,1);
			grid->attach(*item,1,currentYPos,2,1);
			currentYPos++;
		}
	}
	this->add(*grid);
}


