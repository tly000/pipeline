#include "PipelineUI.h"

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
	Platform(Factory factory):
		factory(factory){}

	PipelineWrapper& getPipeline(){
		if(!pipeline){
			pipeline = std::make_unique<MandelPipelineWrapper<Factory,T>>(factory,demangle(typeid(T)));
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
protected:
	CPUImage<unsigned> outputImage{1,1};
	Factory factory;
	std::unique_ptr<MandelPipelineWrapper<Factory,T>> pipeline = nullptr;
	std::unique_ptr<PipelineParameterBox> paramBox = nullptr;
};

MainWindow::MainWindow():
	imageView(this){

	#define addPlatform(T,factory,name) \
		platforms.emplace_back(std::make_unique<Platform<decltype(factory),T>>(factory)); \
		platformMap.insert({ \
			std::make_pair(name,#T),platforms.back().get() \
		});

	CPUFactory factory;
	std::string name = factory.getDeviceName();
	platformBox.append(name);

	addPlatform(float,factory,name);
	addPlatform(double,factory,name);
	addPlatform(Fixed4,factory,name);
	addPlatform(Fixed8,factory,name);
	addPlatform(Fixed16,factory,name);


	for(uint32_t i = 0; i < CLFactory::getNumberOfDevices(); i++){
		CLFactory factory(i);
		std::string name = factory.getDeviceName();
		platformBox.append(name);

		addPlatform(float,factory,name);
		addPlatform(double,factory,name);
		addPlatform(Fixed4,factory,name);
		addPlatform(Fixed8,factory,name);
		addPlatform(Fixed16,factory,name);
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
	header.pack_start(typeBox);

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
