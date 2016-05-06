#include "PipelineUI.h"

/*
 * guitest.cpp
 *
 *  Created on: 02.05.2016
 *      Author: tly
 */


int main() {
	BackTracer::registerErrorHandler();

	auto app = Gtk::Application::create("mandelpipeline");
	auto settings = Gtk::Settings::get_default();
	Glib::ustring fontName = "Sans 10";
	settings->set_property("gtk-font-name",fontName);

	MainWindow window;
	window.set_default_size(500,500);

	app->run(window);
}

