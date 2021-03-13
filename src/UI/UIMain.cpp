#include "PipelineUI.h"
#include <filesystem>
/*
 * guitest.cpp
 *
 *  Created on: 02.05.2016
 *      Author: tly
 */


int main() {
    Glib::setenv("GSETTINGS_SCHEMA_DIR", (std::filesystem::current_path() / "share/glib-2.0/schemas").generic_string());

    auto app = Gtk::Application::create("mandelpipeline");
    auto icon_theme = Gtk::IconTheme::get_default();
    icon_theme->append_search_path((std::filesystem::current_path() / "share/icons").generic_string());

    auto settings = Gtk::Settings::get_default();
    settings->set_property<Glib::ustring>("gtk-font-name", "Sans 10");
    //settings->set_property<Glib::ustring>("gtk-icon-theme-name","elementary");

    MainWindow window;
    window.set_default_size(500, 500);

    app->run(window);
}
