#include "PipelineUI.h"
#include <BackTracer.h>

/*
 * guitest.cpp
 *
 *  Created on: 02.05.2016
 *      Author: tly
 */


symbolexport void uiMain() {
	BackTracer::registerErrorHandler();

	auto app = Gtk::Application::create("mandelpipeline");
	auto settings = Gtk::Settings::get_default();
	settings->set_property<Glib::ustring>("gtk-font-name","Sans 10");
	//settings->set_property<Glib::ustring>("gtk-icon-theme-name","elementary");

	MainWindow window;
	window.set_default_size(500,500);

	app->run(window);
}

