#include "PipelineUI.h"
#include "../MandelPipeline/MandelPipeline.h"
#include "../Platform/CL/CLFactory.h"
#include "../Platform/CPU/CPUFactory.h"
#include "../stb_image_write.h"
#include <fstream>
#include <future>

/*
 * PipelineUI.cpp
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */

MainWindow::MainWindow():
	imageView(this){

	#define addPlatform(T,factory,name) \
		platforms.emplace_back(std::make_unique<Platform<decltype(factory),T>>(factory,#T)); \
		platformMap.insert({ \
			std::make_pair(name,#T),platforms.back().get() \
		});

	CPUFactory factory;
	std::string name = factory.getDeviceName();
	platformBox.append(name);

	addPlatform(float,factory,name);
	addPlatform(double,factory,name);
	//addPlatform(float128,factory,name);
	addPlatform(Fixed4,factory,name);
	addPlatform(Fixed8,factory,name);

	for(uint32_t i = 0; i < CLFactory::getNumberOfDevices(); i++){
		CLFactory factory(i);
		std::string name = factory.getDeviceName();
		platformBox.append(name);

		addPlatform(float,factory,name);
		addPlatform(double,factory,name);
		addPlatform(Fixed4,factory,name);
		addPlatform(Fixed8,factory,name);
	}
	#undef addPlatform

	platformBox.set_active(0);
	platformBox.signal_changed().connect(sigc::mem_fun(*this,&MainWindow::loadPlatform));
	typeBox.signal_changed().connect(sigc::mem_fun(*this,&MainWindow::loadPlatform));

	auto deviceLabel = Gtk::manage(new Gtk::Label("Device:"));
	header.pack_start(*deviceLabel);
	header.pack_start(platformBox);

	header.pack_start(*Gtk::manage(new Gtk::Label("Type:")));
	typeBox.append("float");
	typeBox.append("double");
	typeBox.append("Fixed4");
	typeBox.append("Fixed8");
	typeBox.append("float128");
	header.pack_start(typeBox);

	auto openButton = Gtk::manage(new Gtk::Button());
	openButton->set_image_from_icon_name("document-open-symbolic");
	openButton->signal_clicked().connect([this]{
		Gtk::FileChooserDialog dialog(*this,"Open settings",Gtk::FILE_CHOOSER_ACTION_OPEN);
		dialog.set_current_folder("./settings");
		dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
		dialog.add_button("_Open", Gtk::RESPONSE_ACCEPT);
		auto filter = Gtk::FileFilter::create();
		filter->set_name("settings file (*.json)");
		filter->add_pattern("*.json");
		dialog.add_filter(filter);
		if(dialog.run() == Gtk::RESPONSE_ACCEPT){
			std::string fileName = dialog.get_filename();
			std::string jsonObj = fileToString(fileName);
			try{
				this->getSelectedPlatform()->paramsFromJson(jsonObj);
			}catch(std::exception& e){
				Gtk::MessageDialog messageBox(*this,std::string("error while loading settings: \n") + e.what());
				messageBox.run();
			}
		}
	});
	header.pack_end(*openButton);
	auto popOverBox = Gtk::manage(new Gtk::Box(Gtk::ORIENTATION_VERTICAL));
		auto saveImageButton = Gtk::manage(new Gtk::Button());
		saveImageButton->set_image_from_icon_name("camera-photo-symbolic");
		saveImageButton->signal_clicked().connect([this]{
			Gtk::FileChooserDialog dialog(*this,"Save image",Gtk::FILE_CHOOSER_ACTION_SAVE);
		    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
		    dialog.add_button("_Save", Gtk::RESPONSE_ACCEPT);
			auto filter = Gtk::FileFilter::create();
			filter->set_name("image file (*.bmp,*.png)");
			filter->add_pattern("*.bmp");
			filter->add_pattern("*.png");
			dialog.add_filter(filter);
			if(dialog.run() == Gtk::RESPONSE_ACCEPT){
				std::string fileName = dialog.get_filename();
				auto& image = this->getSelectedPlatform()->getRenderedImage();
				if(fileName.substr(fileName.size()-4) == ".bmp"){
					stbi_write_bmp(fileName.c_str(),image.getWidth(),image.getHeight(),4,image.getDataPointer());
				}else if(fileName.substr(fileName.size()-4) == ".png"){
					stbi_write_png(fileName.c_str(),image.getWidth(),image.getHeight(),4,image.getDataPointer(),0);
				}else{
					fileName += ".png";
					stbi_write_png(fileName.c_str(),image.getWidth(),image.getHeight(),4,image.getDataPointer(),0);
				}
			}
		});
		popOverBox->add(*saveImageButton);
		auto saveSettingsButton = Gtk::manage(new Gtk::Button());
		saveSettingsButton->set_image_from_icon_name("preferences-system-symbolic");
		saveSettingsButton->signal_clicked().connect([this]{
			struct Columns : Gtk::TreeStore::ColumnRecord{
				Columns(){
					this->add(paramName);
					this->add(selected);
				}
				Gtk::TreeModelColumn<Glib::ustring> paramName;
				Gtk::TreeModelColumn<bool> selected;
			} record;
			auto treeStore = Gtk::TreeStore::create(record);
			Gtk::TreeView treeView(treeStore);

			for(auto& paramPack : this->getSelectedPlatform()->getPipeline().getParamPacks()){
				auto row = *treeStore->append();
				row[record.paramName] = paramPack.first;
				row[record.selected] = true;

				for(auto& param : *paramPack.second){
					auto childrow = *treeStore->append(row.children());
					childrow[record.paramName] = param->name;
					childrow[record.selected] = true;
				}
			}

			treeStore->signal_row_changed().connect([&](const Gtk::TreeModel::Path& p,const Gtk::TreeModel::iterator&){
				if(p.size() == 1){
					auto& row = *treeStore->get_iter(p);
					bool isSelected = row[record.selected];
					for(auto& child : row.children()){
						child[record.selected] = isSelected;
					}
				}
			});

			treeView.append_column_editable("", record.selected);
			treeView.append_column("", record.paramName);
			treeView.expand_all();

			Gtk::Dialog selectionDialog("select settings",*this);
			selectionDialog.get_content_area()->add(*Gtk::manage(new Gtk::Label("select settings to save:")));
			selectionDialog.get_content_area()->add(treeView);
			selectionDialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
			selectionDialog.add_button("_Ok", Gtk::RESPONSE_ACCEPT);
			selectionDialog.show_all_children();
			if(selectionDialog.run() != Gtk::RESPONSE_ACCEPT){
				return;
			}

			std::map<std::string,std::list<std::string>> selectedParams;
			for(auto& paramPackChild : treeStore->children()){
				std::list<std::string> selected;
				for(auto& paramChild : paramPackChild.children()){
					if(paramChild[record.selected]){
						selected.push_back(Glib::ustring(paramChild[record.paramName]));
					}
				}
				if(selected.size()){
					selectedParams.emplace(
						Glib::ustring(paramPackChild[record.paramName]),
						selected
					);
				}
			}

			Gtk::FileChooserDialog dialog(*this,"Save settings",Gtk::FILE_CHOOSER_ACTION_SAVE);
		    dialog.add_button("_Cancel", Gtk::RESPONSE_CANCEL);
		    dialog.add_button("_Save", Gtk::RESPONSE_ACCEPT);
			auto filter = Gtk::FileFilter::create();
			filter->set_name("json file (*.json)");
			filter->add_pattern("*.json");
			dialog.add_filter(filter);
			if(dialog.run() != Gtk::RESPONSE_ACCEPT){
				return;
			}
			std::string fileName = dialog.get_filename();
			std::string object = this->getSelectedPlatform()->paramsToJson(selectedParams);
			if(fileName.substr(fileName.size()-5) != ".json"){
				fileName += ".json";
			}
			std::ofstream file(fileName);
			if(file){
				file << object;
				file.close();
			}else{
				Gtk::MessageDialog messageBox(*this,"could not save json file.");
				messageBox.run();
			}
		});
		popOverBox->add(*saveSettingsButton);
		popOverBox->show_all();
	auto popOver = Gtk::manage(new Gtk::Popover());
	popOver->add(*popOverBox);
	saveButton.set_image_from_icon_name("media-floppy-symbolic");
	saveButton.set_popover(*popOver);
	calcbuttonConnection = calculateButton.signal_clicked().connect([this]{
		this->calculateNow();
	});
	header.pack_end(saveButton);
	header.pack_end(calculateButton);

	verticalBox.add(header);
	verticalBox.add(mainView);

	mainView.add1(imageView);
	mainView.add2(parameterBox);

	this->add(verticalBox);

	typeBox.set_active(0);
	calculateButton.clicked();

	//set scroll policy
	parameterBox.set_policy(Gtk::POLICY_NEVER,Gtk::POLICY_AUTOMATIC);
	this->show_all();
}

void MainWindow::setSensitive(bool s){
	this->parameterBox.set_sensitive(s);
	for(auto& c : this->header.get_children()){
		c->set_sensitive(s);
	}
	this->imageView.set_sensitive(s);
}

void MainWindow::calculateNow() {
	std::shared_ptr<std::string> error = std::make_shared<std::string>("");
	std::shared_ptr<std::atomic_bool> showingImage = std::make_shared<atomic_bool>(false);
	std::shared_ptr<std::atomic_bool> done = std::make_shared<atomic_bool>(false);
	std::shared_ptr<std::atomic_bool> cancel = std::make_shared<atomic_bool>(false);

	Glib::signal_idle().connect([=]{
		if(*done){
			if(error->size()){
				Gtk::MessageDialog messageBox(*this,*error);
				messageBox.run();
			}
			this->setSensitive(true);
			this->calculateButton.set_label("calculate");
			calcbuttonConnection.disconnect();
			calcbuttonConnection = this->calculateButton.signal_clicked().connect([=]{
				this->calculateNow();
			});
			return false;
		}
		if(*showingImage){
			this->imageView.updateView(selectedPlatform->getRenderedImage());
			*showingImage = false;
		}
		return true;
	});

	this->setSensitive(false);
	this->calculateButton.set_sensitive(true);
	this->calculateButton.set_label("cancel");
	calcbuttonConnection.disconnect();
	calcbuttonConnection = this->calculateButton.signal_clicked().connect([=]{
		*cancel = true;
	});

	std::thread([=]{
		try{
			this->selectedPlatform->setReset(true);
			do{
				if(*cancel){
					break;
				}
				this->selectedPlatform->getPipeline().run();

				*showingImage = true;
				while(*showingImage);

				this->selectedPlatform->setReset(false);
			}while(!this->selectedPlatform->isDone());
		}catch(const std::exception& e){
			*error = e.what();
		}
		*done = true;
	}).detach();
}

void MainWindow::loadPlatform() {
	if(parameterBox.get_children().size()){
		parameterBox.remove();
	}
	std::string platformName = platformBox.get_active_text();
	std::string typeName = typeBox.get_active_text();
	auto accessPair = std::make_pair(platformName,typeName);

	auto platform = this->platformMap.at(accessPair);
	if(this->selectedPlatform != nullptr){
		for(auto& pack : this->selectedPlatform->getPipeline().getParamPacks()){
			std::string packname = pack.first;
			auto& packOfNewPlatform = platform->getPipeline().getParamPack(packname);
			for(auto& param : *pack.second){
				packOfNewPlatform.getParam(param->name).setValueFromString(param->getValueAsString());
			}
		}
	}
	this->selectedPlatform = platform;

	parameterBox.add(platform->getParameterBox());
	parameterBox.show_all();

}
