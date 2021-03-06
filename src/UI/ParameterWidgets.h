#pragma once
#include "../Type/EnumClass.h"

/*
 * ParameterWidgets.h
 *
 *  Created on: 30.05.2016
 *      Author: tly
 */

template<typename T>
struct ParameterWidget<T> : Gtk::Entry {
    ParameterWidget(Parameter &param) : param(param) {
        this->add_events(Gdk::EventMask::KEY_PRESS_MASK);
        this->set_text(param.getValueAsString());
        this->signal_focus_out_event().connect([this](GdkEventFocus *) {
            this->commitChange();
            return false;
        });
        this->signal_key_press_event().connect([this](GdkEventKey *e) {
            if (e->keyval == GDK_KEY_Return) { this->commitChange(); }
            return false;
        });
        this->param.registerObserver([this](Parameter *) { this->set_text(this->param.getValueAsString()); });
    }

protected:
    Parameter &param;

    void commitChange() {
        if (!this->param.setValueFromString(this->get_text().raw())) { this->set_text(this->param.getValueAsString()); }
    }
};

template<typename T>
struct ParameterWidget<T, std::enable_if_t<std::is_integral<T>::value>> : Gtk::SpinButton {
    using Limits = std::numeric_limits<T>;

    ParameterWidget(TypedParameter<T> &param)
        : Gtk::SpinButton(Gtk::Adjustment::create(0, Limits::min(), Limits::max()), 1), param(param) {
        this->add_events(Gdk::EventMask::KEY_PRESS_MASK);
        this->set_text(param.getValueAsString());
        this->signal_focus_out_event().connect([this](GdkEventFocus *) {
            this->commitChange();
            return false;
        });
        this->signal_key_press_event().connect([this](GdkEventKey *e) {
            if (e->keyval == GDK_KEY_Return) { this->commitChange(); }
            return false;
        });
        this->param.registerObserver([this](Parameter *) { this->set_text(this->param.getValueAsString()); });
    }

protected:
    TypedParameter<T> &param;

    void commitChange() {
        if (!this->param.setValueFromString(this->get_text().raw())) { this->set_text(this->param.getValueAsString()); }
    }
};

template<>
struct ParameterWidget<bool> : Gtk::CheckButton {
    ParameterWidget(TypedParameter<bool> &param) : param(param) {
        this->add_events(Gdk::EventMask::KEY_PRESS_MASK);
        this->set_active(param.getValue());
        this->signal_toggled().connect([this] { this->commitChange(); });
        this->param.registerObserver([this](Parameter *) { this->set_active(this->param.getValue()); });
    }

protected:
    TypedParameter<bool> &param;

    void commitChange() { this->param.setValue(this->get_active()); }
};

template<typename... Strings>
struct ParameterWidget<StringEnum<Strings...>> : Gtk::ComboBoxText {
    ParameterWidget(TypedParameter<StringEnum<Strings...>> &param) : param(param) {
        variadicForEach(this->append(Strings::toString()));
        this->set_active(this->param.getValue().getIndex());

        this->param.registerObserver([this](Parameter *) { this->set_active(this->param.getValue().getIndex()); });

        this->signal_changed().connect([this] {
            auto val = this->param.getValue();
            val.setIndex(this->get_active_row_number());
            this->param.setValue(val);
        });
    }

protected:
    TypedParameter<StringEnum<Strings...>> &param;
};

#include "../Type/Gradient.h"

template<>
struct ParameterWidget<Gradient> : Gtk::FlowBox {
    ParameterWidget(TypedParameter<Gradient> &param) : param(param) {
        param.registerObserver([this](Parameter *) { this->loadChange(); });
        this->loadChange();
    }

protected:
    TypedParameter<Gradient> &param;

    void saveChange() {
        Gradient g;
        for (auto *child : this->get_children()) {
            if (auto *flowboxChild = dynamic_cast<Gtk::FlowBoxChild *>(child)) {
                if (auto *colorButton = dynamic_cast<Gtk::ColorButton *>(flowboxChild->get_child())) {
                    auto c = colorButton->get_color();
                    g.push_back({float(c.get_red_p()), float(c.get_green_p()), float(c.get_blue_p())});
                }
            }
        }
        param.setValue(g);
    }

    void loadChange() {
        for (auto *child : this->get_children()) { this->remove(*child); }
        for (auto &color : this->param.getValue()) {
            Gdk::Color c;
            c.set_rgb_p(color[0], color[1], color[2]);
            auto colorButton = Gtk::manage(new Gtk::ColorButton(c));
            this->add(*colorButton);
            colorButton->signal_color_set().connect([this] { this->saveChange(); });
        }
        auto removeButton = Gtk::manage(new Gtk::Button());
        removeButton->set_image_from_icon_name("list-remove-symbolic");
        removeButton->signal_clicked().connect([this]() {
            if (this->get_children().size() > 2) { this->removeColor(); }
        });
        this->add(*removeButton);
        auto addButton = Gtk::manage(new Gtk::Button());
        addButton->set_image_from_icon_name("list-add-symbolic");
        addButton->signal_clicked().connect([this]() { this->addColor(); });
        this->add(*addButton);
        this->show_all();
    }

    void addColor() {
        Gradient g = this->param.getValue();
        g.push_back({1, 1, 1});
        this->param.setValue(g);
    }

    void removeColor() {
        Gradient g = this->param.getValue();
        g.pop_back();
        this->param.setValue(g);
    }
};

#include "../Platform/Factory.h"
#include "CurveEditor.h"

template<>
struct ParameterWidget<Curve> : Gtk::Box {
    ParameterWidget(TypedParameter<Curve> &param) : param(param) {
        auto button = Gtk::manage(new Gtk::Button("edit curve"));
        this->add(*button);
        button->signal_clicked().connect([this] {
            CurveEditor editor;
            editor.setCurve(this->param.getValue());
            Gradient g;
            if (this->param.name == "outer curve") {
                auto &gradParam = this->param.getParamPack().getParam("outer gradient");
                g = dynamic_cast<TypedParameter<Gradient> &>(gradParam).getValue();
            } else if (this->param.name == "inner curve") {
                auto &gradParam = this->param.getParamPack().getParam("inner gradient");
                g = dynamic_cast<TypedParameter<Gradient> &>(gradParam).getValue();
            } else {
                g = {{1, 0, 0}, {1, 1, 0}, {0, 1, 0}, {0, 1, 1}, {0, 0, 1}, {1, 0, 1}, {1, 0, 0}};
            }
            editor.setGradient(g);

            Gtk::Dialog curveDialog("edit curve", *(Gtk::Window *) this->get_toplevel());
            curveDialog.get_content_area()->add(editor);
            curveDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
            curveDialog.add_button("_Ok", Gtk::RESPONSE_ACCEPT);
            curveDialog.show_all_children();
            if (curveDialog.run() == Gtk::RESPONSE_ACCEPT) { this->param.setValue(editor.getCurve()); }
        });
    }

protected:
    TypedParameter<Curve> &param;
};

template<>
struct ParameterWidget<std::shared_ptr<Factory>> : Gtk::ComboBoxText {
    ParameterWidget(TypedParameter<std::shared_ptr<Factory>> &param) : param(param) {
        for (const auto &factory : Factory::getAllFactories()) { this->append(factory->getDeviceName()); }
        this->set_active_text(param.getValueAsString());

        this->param.registerObserver([this](Parameter *) { this->set_active_text(this->param.getValueAsString()); });
        this->signal_changed().connect([this] { this->param.setValueFromString(this->get_active_text()); });
    }

protected:
    TypedParameter<std::shared_ptr<Factory>> &param;
};
