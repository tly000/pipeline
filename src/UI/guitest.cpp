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
	MainWindow window;
	window.set_default_size(500,500);

	app->run(window);
}

