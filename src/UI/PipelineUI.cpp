#include "PipelineUI.h"
#include <fstream>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "../stb_image_write.h"

/*
 * PipelineUI.cpp
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */
template<typename Factory,typename T>
struct Platform : AbstractPlatform{
	Platform(Factory factory,std::string typeName):
		typeName(typeName),
		factory(factory){}

	PipelineWrapper& getPipeline(){
		if(!pipeline){
			pipeline = std::make_unique<MandelPipelineWrapper<Factory,T>>(factory,typeName);
		}
		return *pipeline;
	}

	PipelineParameterBox& getParameterBox(){
		if(!paramBox){
			paramBox = std::make_unique<PipelineParameterBox>(this->getPipeline());
		}
		return *paramBox;
	}

	const CPUImage<unsigned>& getRenderedImage(){
		auto& renderedImage = pipeline->getRenderedImage();
		if(outputImage.getHeight() != renderedImage.getHeight() ||
		   outputImage.getWidth() != renderedImage.getWidth()){
			outputImage = CPUImage<unsigned>(renderedImage.getWidth(),renderedImage.getHeight());
		}
		renderedImage.copyToBuffer(outputImage.getDataBuffer());
		return outputImage;
	}

	void scale(float factor){
		//get current scale.
		auto& param = this->pipeline->getPositionParam();
		auto scale = param.template getValue<2>();

		param.template getParam<2>().setValue(tmul(scale,fromFloatToType<decltype(scale)>(factor)));
	}

	//translate by x,y in image space
	void translate(float x,float y){
		//get current position and scale.
		auto& param = this->pipeline->getPositionParam();
		auto offsetReal = param.template getValue<0>();
		auto offsetImag = param.template getValue<1>();
		auto scale = param.template getValue<2>();
		float rotationAngle = param.template getValue<3>() / 180 * G_PI;

		auto cosA = fromFloatToType<decltype(scale)>(std::cos(rotationAngle));
		auto sinA = fromFloatToType<decltype(scale)>(std::sin(rotationAngle));
		auto X = tmul(scale,fromFloatToType<decltype(scale)>(x));
		auto Y = tmul(scale,fromFloatToType<decltype(scale)>(y));
		auto newX = tsub(tmul(X,cosA),tmul(Y,sinA));
		auto newY = tadd(tmul(X,sinA),tmul(Y,cosA));

		param.template getParam<0>().setValue(tadd(offsetReal,newX));
		param.template getParam<1>().setValue(tadd(offsetImag,newY));
	}

	//translate by x,y in image space
	void rotate(float angle){
		//get current position and scale.
		auto& param = this->pipeline->getPositionParam();
		float currentAngle = param.template getParam<3>().getValue();
		param.template  getParam<3>().setValue(currentAngle + angle);
	}

	std::string getName() const {
		return factory.getDeviceName();
	}

	std::string paramsToJson(const std::map<std::string,std::list<std::string>>& paramsToSave){
		std::stringstream str;

		bool firstPack = true;
		str << "{\n";
		for(auto& pack : paramsToSave){
			if(!firstPack){
				str << ",\n";
			}else{
				firstPack = false;
			}
			str << "\t\"" << pack.first << "\"" << " : {\n";
			ParamPack& p = this->getPipeline().getParamPack(pack.first);

			bool firstParam = true;
			for(auto& paramName : pack.second){
				if(!firstParam){
					str << ",\n";
				}else{
					firstParam = false;
				}
				Parameter& param = p.getParam(paramName);
				str << "\t\t\"" << param.name << "\"" << " : " << "\"" << param.getValueAsString() << "\"";
			}
			str << "\n\t}";
		}
		str << "\n}";
		return str.str();
	}

	void paramsFromJson(const std::string& jsonObj){
		ParseTree obj = JsonParser::parseJson(jsonObj);
		_log("[info] json: " << printTree(obj));
		auto unquote = [](const std::string& s){
			return s.substr(1,s.size()-2);
		};

		assertOrThrow(obj.elementName == "object");
		for(auto& child : obj.children){
			assertOrThrow(child.elementName == "pair" && child.children.back().elementName == "object");
			std::string paramPackName = unquote(child.children.front().children.front().elementName);
			ParamPack& pack = this->getPipeline().getParamPack(paramPackName);
			auto& object = child.children.back();

			for(auto& param : object.children){
				std::string name = unquote(param.children.front().children.front().elementName);
				if(pack.hasParam(name)){
					auto& value = param.children.back();
					assertOrThrow(value.elementName == "string");
					assertOrThrow(value.elementName == "string");
					if(!pack.getParam(name).setValueFromString(unquote(value.children.front().elementName))){
						throw std::runtime_error("invalid value for parameter " + name);
					}
				}else{
					throw std::runtime_error("could not find parameter with name " + name + " inside pack with name " + paramPackName);
				}
			}
		}
	}
protected:
	std::string typeName;
	CPUImage<unsigned> outputImage{1,1};
	Factory factory;
	std::unique_ptr<MandelPipelineWrapper<Factory,T>> pipeline = nullptr;
	std::unique_ptr<PipelineParameterBox> paramBox = nullptr;
};

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
	addPlatform(float128,factory,name);
	addPlatform(Fixed4,factory,name);
	addPlatform(Fixed8,factory,name);
	addPlatform(Fixed16,factory,name);
	addPlatform(longdouble,factory,name);

	for(uint32_t i = 0; i < CLFactory::getNumberOfDevices(); i++){
		CLFactory factory(i);
		std::string name = factory.getDeviceName();
		platformBox.append(name);

		addPlatform(float,factory,name);
		addPlatform(double,factory,name);
		addPlatform(float128,factory,name);
		addPlatform(Fixed4,factory,name);
		addPlatform(Fixed8,factory,name);
		addPlatform(Fixed16,factory,name);
		addPlatform(longdouble,factory,name);
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
	typeBox.append("Fixed16");
	typeBox.append("float128");
	typeBox.append("longdouble");
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
	calculateButton.signal_clicked().connect([this]{
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

void MainWindow::calculateNow() {
	try{
		selectedPlatform->getPipeline().run();
		this->imageView.updateView(selectedPlatform->getRenderedImage());
	}catch(std::exception& e){
		Gtk::MessageDialog messageBox(*this,e.what());
		messageBox.run();
	}
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
