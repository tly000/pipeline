#include "PipelineUI.h"
#include "../MandelPipeline/MandelPipeline.h"
#include "../stb_image_write.h"
#include <fstream>
#include <future>

/*
 * PipelineUI.cpp
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */

MainWindow::MainWindow() : imageView(this) {
    auto openButton = Gtk::manage(new Gtk::Button());
    openButton->set_image_from_icon_name("document-open-symbolic");
    openButton->signal_clicked().connect([this] {
        Gtk::FileChooserDialog dialog(*this, "Open settings", Gtk::FILE_CHOOSER_ACTION_OPEN);
        dialog.set_current_folder("./settings");
        dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Open", Gtk::RESPONSE_ACCEPT);
        auto filter = Gtk::FileFilter::create();
        filter->set_name("settings file (*.json)");
        filter->add_pattern("*.json");
        dialog.add_filter(filter);
        if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
            std::string fileName = dialog.get_filename();
            std::string jsonObj = fileToString(fileName);
            try {
                this->pipeline.paramsFromJson(jsonObj);
            } catch (std::exception &e) {
                Gtk::MessageDialog messageBox(*this, std::string("error while loading settings: \n") + e.what());
                messageBox.run();
            }
        }
    });
    header.pack_start(*openButton);
    auto popOverBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
    auto saveImageButton = Gtk::manage(new Gtk::Button());
    saveImageButton->set_image_from_icon_name("camera-photo-symbolic");
    saveImageButton->signal_clicked().connect([this] {
        Gtk::FileChooserDialog dialog(*this, "Save image", Gtk::FILE_CHOOSER_ACTION_SAVE);
        dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Save", Gtk::RESPONSE_ACCEPT);
        auto filter = Gtk::FileFilter::create();
        filter->set_name("image file (*.bmp,*.png)");
        filter->add_pattern("*.bmp");
        filter->add_pattern("*.png");
        dialog.add_filter(filter);
        if (dialog.run() == Gtk::RESPONSE_ACCEPT) {
            std::string fileName = dialog.get_filename();
            auto &image = this->pipeline.reductionAction.getOutput(_C("reducedImage")).getValue();
            std::vector<std::uint32_t> image_data(image.getWidth() * image.getHeight());
            image.getRawImage()->copyToBuffer(image_data.data(), sizeof(std::uint32_t) * image_data.size());
            if (fileName.substr(fileName.size() - 4) == ".bmp") {
                stbi_write_bmp(fileName.c_str(), image.getWidth(), image.getHeight(), 4, image_data.data());
            } else if (fileName.substr(fileName.size() - 4) == ".png") {
                stbi_write_png(fileName.c_str(), image.getWidth(), image.getHeight(), 4, image_data.data(), 0);
            } else {
                fileName += ".png";
                stbi_write_png(fileName.c_str(), image.getWidth(), image.getHeight(), 4, image_data.data(), 0);
            }
        }
    });
    popOverBox->add(*saveImageButton);
    auto saveSettingsButton = Gtk::manage(new Gtk::Button());
    saveSettingsButton->set_image_from_icon_name("preferences-system-symbolic");
    saveSettingsButton->signal_clicked().connect([this] {
        struct Columns : Gtk::TreeStore::ColumnRecord {
            Columns() {
                this->add(paramName);
                this->add(selected);
            }
            Gtk::TreeModelColumn<Glib::ustring> paramName;
            Gtk::TreeModelColumn<bool> selected;
        } record;
        auto treeStore = Gtk::TreeStore::create(record);
        Gtk::TreeView treeView(treeStore);

        for (auto &paramPack : this->pipeline.getParamPacks()) {
            auto row = *treeStore->append();
            row[record.paramName] = paramPack.first;
            row[record.selected] = true;

            for (auto &param : *paramPack.second) {
                auto childrow = *treeStore->append(row.children());
                childrow[record.paramName] = param->name;
                childrow[record.selected] = true;
            }
        }

        treeStore->signal_row_changed().connect([&](const Gtk::TreeModel::Path &p, const Gtk::TreeModel::iterator &) {
            if (p.size() == 1) {
                auto &row = *treeStore->get_iter(p);
                bool isSelected = row[record.selected];
                for (auto &child : row.children()) { child[record.selected] = isSelected; }
            }
        });

        treeView.append_column_editable("", record.selected);
        treeView.append_column("", record.paramName);
        treeView.expand_all();

        Gtk::Dialog selectionDialog("select settings", *this);
        selectionDialog.get_content_area()->add(*Gtk::manage(new Gtk::Label("select settings to save:")));
        selectionDialog.get_content_area()->add(treeView);
        selectionDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        selectionDialog.add_button("_Ok", Gtk::RESPONSE_ACCEPT);
        selectionDialog.show_all_children();
        if (selectionDialog.run() != Gtk::RESPONSE_ACCEPT) { return; }

        std::map<std::string, std::list<std::string>> selectedParams;
        for (auto &paramPackChild : treeStore->children()) {
            std::list<std::string> selected;
            for (auto &paramChild : paramPackChild.children()) {
                if (paramChild[record.selected]) { selected.push_back(Glib::ustring(paramChild[record.paramName])); }
            }
            if (selected.size()) { selectedParams.emplace(Glib::ustring(paramPackChild[record.paramName]), selected); }
        }

        Gtk::FileChooserDialog dialog(*this, "Save settings", Gtk::FILE_CHOOSER_ACTION_SAVE);
        dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
        dialog.add_button("_Save", Gtk::RESPONSE_ACCEPT);
        auto filter = Gtk::FileFilter::create();
        filter->set_name("json file (*.json)");
        filter->add_pattern("*.json");
        dialog.add_filter(filter);
        if (dialog.run() != Gtk::RESPONSE_ACCEPT) { return; }
        std::string fileName = dialog.get_filename();
        std::string object = this->pipeline.paramsToJson(selectedParams);
        if (fileName.substr(fileName.size() - 5) != ".json") { fileName += ".json"; }
        std::ofstream file(fileName);
        if (file) {
            file << object;
            file.close();
        } else {
            Gtk::MessageDialog messageBox(*this, "could not save json file.");
            messageBox.run();
        }
    });
    popOverBox->add(*saveSettingsButton);
    popOverBox->show_all();
    auto popOver = Gtk::manage(new Gtk::Popover());
    popOver->add(*popOverBox);
    saveButton.set_image_from_icon_name("media-floppy-symbolic");
    saveButton.set_popover(*popOver);
    calcbuttonConnection = calculateButton.signal_clicked().connect([this] { this->calculateNow(); });
    header.pack_start(saveButton);
    header.pack_start(calculateButton);
    header.set_show_close_button(true);
    header.set_title("mandelpipeline");

    set_titlebar(header);

    //set scroll policy
    parameterBox.set_policy(Gtk::POLICY_NEVER, Gtk::POLICY_AUTOMATIC);
    parameterBox.add(*Gtk::manage(new PipelineParameterBox(pipeline)));
    parameterBox.show_all();

    mainView.add1(imageView);
    mainView.add2(parameterBox);

    this->add(mainView);

    calculateButton.clicked();

    this->show_all();
}

void MainWindow::setSensitive(bool s) {
    this->parameterBox.set_sensitive(s);
    for (auto &c : this->header.get_children()) { c->set_sensitive(s); }
    this->imageView.set_sensitive(s);
}

void MainWindow::calculateNow() {
    std::shared_ptr<std::string> error = std::make_shared<std::string>("");
    std::shared_ptr<std::atomic_bool> done = std::make_shared<atomic_bool>(false);
    std::shared_ptr<std::atomic_bool> cancel = std::make_shared<atomic_bool>(false);

    Glib::signal_idle().connect([=] {
        if (*done) {
            if (error->size()) {
                Gtk::MessageDialog messageBox(*this, *error);
                messageBox.run();
            }
            this->setSensitive(true);
            this->calculateButton.set_label("calculate");
            calcbuttonConnection.disconnect();
            calcbuttonConnection = this->calculateButton.signal_clicked().connect([=] { this->calculateNow(); });
            return false;
        }
        return true;
    });

    this->setSensitive(false);
    this->calculateButton.set_sensitive(true);
    this->calculateButton.set_label("cancel");
    calcbuttonConnection.disconnect();
    calcbuttonConnection = this->calculateButton.signal_clicked().connect([=] { *cancel = true; });

    std::thread([=] {
        try {
            this->pipeline.calcAction.getInput(_C("reset calculation")).setDefaultValue(true);
            do {
                if (*cancel) { break; }
                this->pipeline.run();
                this->imageView.updateView(pipeline.reductionAction.getOutput(_C("reducedImage")).getValue());
                this->pipeline.calcAction.getInput(_C("reset calculation")).setDefaultValue(false);
            } while (!this->pipeline.calcAction.getOutput(_C("done")).getValue());
        } catch (const std::exception &e) { *error = e.what(); }
        *done = true;
    }).detach();
}
