#include "PipelineParameterBox.h"

/*
 * PipelineParameterBox.cpp
 *
 *  Created on: 03.05.2016
 *      Author: tly
 */

template<typename T,typename = void>
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


template<typename T> struct ParameterItem<T,std::enable_if_t<std::is_integral<T>::value>> : Gtk::SpinButton{
	using Limits = std::numeric_limits<T>;

	ParameterItem(TypedParameter<T>& param)
	:Gtk::SpinButton(Gtk::Adjustment::create(0,Limits::min(),Limits::max()),1),
	 param(param){
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
	TypedParameter<T>& param;

	void commitChange(){
		if(!this->param.setValueFromString(this->get_text().raw())){
			this->set_text(this->param.getValueAsString());
		}
	}
};

template<> struct ParameterItem<bool> : Gtk::CheckButton{

	ParameterItem(TypedParameter<bool>& param)
	:param(param){
		this->add_events(Gdk::EventMask::KEY_PRESS_MASK);
		this->set_active(param.getValue());
		this->signal_toggled().connect([this]{
			this->commitChange();
		});
		this->param.registerObserver([this](Parameter*){
			this->set_active(this->param.getValue());
		});
	}
protected:
	TypedParameter<bool>& param;

	void commitChange(){
		this->param.setValue(this->get_active());
	}
};

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
			Gtk::Widget* item = nullptr;

			auto paramCase = [&](auto v){
				if(auto* p = dynamic_cast<TypedParameter<decltype(v)>*>(param.get())){
					item = Gtk::manage(
						new ParameterItem<decltype(v)>(*p)
					);
					return true;
				}
				return false;
			};

			if(!paramCase(int32_t()) &&
			   !paramCase(uint32_t())&&
			   !paramCase(bool())
			){
				item = Gtk::manage(
					new ParameterItem<std::string>(*param)
				);
			}

			#undef paramCase
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


