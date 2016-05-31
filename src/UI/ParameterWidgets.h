#pragma once
#include "../Type/EnumClass.h"

/*
 * ParameterWidgets.h
 *
 *  Created on: 30.05.2016
 *      Author: tly
 */

template<typename T> struct ParameterWidget<T>: Gtk::Entry {
	ParameterWidget(Parameter& param) :
			param(param) {
		this->add_events(Gdk::EventMask::KEY_PRESS_MASK);
		this->set_text(param.getValueAsString());
		this->signal_focus_out_event().connect([this](GdkEventFocus*) {
			this->commitChange();
			return false;
		});
		this->signal_key_press_event().connect([this](GdkEventKey* e) {
			if(e->keyval == GDK_KEY_Return) {
				this->commitChange();
			}
			return false;
		});
		this->param.registerObserver([this](Parameter*) {
			this->set_text(this->param.getValueAsString());
		});
	}
protected:
	Parameter& param;

	void commitChange() {
		if (!this->param.setValueFromString(this->get_text().raw())) {
			this->set_text(this->param.getValueAsString());
		}
	}
};

template<typename T> struct ParameterWidget<T,std::enable_if_t<std::is_integral<T>::value>> : Gtk::SpinButton{
	using Limits = std::numeric_limits<T>;

	ParameterWidget(TypedParameter<T>& param)
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

template<> struct ParameterWidget<bool> : Gtk::CheckButton{
	ParameterWidget(TypedParameter<bool>& param)
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

template<size_t N, const char* const Strings[N]> struct ParameterWidget<StringEnum<N,Strings>> : Gtk::ComboBoxText{
	ParameterWidget(TypedParameter<StringEnum<N,Strings>>& param)
		:param(param){
			for(uint32_t i = 0; i < N; i++){
				this->append(Strings[i]);
			}
			this->set_active(this->param.getValue().getIndex());

			this->param.registerObserver([this](Parameter*){
				this->set_active(this->param.getValue().getIndex());
			});

			this->signal_changed().connect([this]{
				auto val = this->param.getValue();
				val.setIndex(this->get_active_row_number());
				this->param.setValue(val);
			});
		}
protected:
	TypedParameter<StringEnum<N,Strings>>& param;
};

