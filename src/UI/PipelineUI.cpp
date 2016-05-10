#include "PipelineUI.h"

/*
 * PipelineUI.cpp
 *
 *  Created on: 04.05.2016
 *      Author: tly
 */

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
	//addPlatform(Q16_16,factory,name);
	//addPlatform(Q32_32,factory,name);
	addPlatform(Fixed4,factory,name);
	addPlatform(Fixed8,factory,name);
	addPlatform(Fixed16,factory,name);


	for(uint32_t i = 0; i < CLFactory::getNumberOfDevices(); i++){
		CLFactory factory(i);
		std::string name = factory.getDeviceName();
		platformBox.append(name);

		addPlatform(float,factory,name);
		addPlatform(double,factory,name);
		//addPlatform(Q16_16,factory,name);
		//addPlatform(Q32_32,factory,name);
		addPlatform(Fixed4,factory,name);
		addPlatform(Fixed8,factory,name);
		addPlatform(Fixed16,factory,name);
	}
	#undef addPlatform

	platformBox.set_active(0);
	platformBox.signal_changed().connect(sigc::mem_fun(*this,&MainWindow::loadPlatform));
	typeBox.signal_changed().connect(sigc::mem_fun(*this,&MainWindow::loadPlatform));

	header.pack_start(*Gtk::manage(new Gtk::Label("Device:")));
	header.pack_start(platformBox);

	header.pack_start(*Gtk::manage(new Gtk::Label("Type:")));
	typeBox.append("float");
	typeBox.append("double");
	//typeBox.append("Q16_16");
	//typeBox.append("Q32_32");
	typeBox.append("Fixed4");
	typeBox.append("Fixed8");
	typeBox.append("Fixed16");
	header.pack_start(typeBox);

	calculateButton.signal_clicked().connect([this]{
		this->calculateNow();
	});
	header.pack_end(calculateButton);

	verticalBox.add(header);
	verticalBox.add(mainView);

	mainView.add1(imageView);
	mainView.add2(parameterBox);

	this->add(verticalBox);

	typeBox.set_active(0);
	calculateButton.clicked();

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
		parameterBox.remove(*parameterBox.get_children().front());
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
