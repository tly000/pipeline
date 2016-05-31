#pragma once
#include "../Actions/ParameterAction.h"
#include "../Type/TypeHelper.h"
#include <map>
#include "GtkHeader.h"

/*
 * NamedParameterAction.h
 *
 *  Created on: 03.05.2016
 *      Author: tly
 */

struct Parameter{
	Parameter(std::string name) : name(name){}

	virtual bool setValueFromString(std::string val) = 0;
	virtual std::string getValueAsString() const = 0;

	virtual ~Parameter() = default; //making it virtual
	const std::string name;

	void registerObserver(std::function<void(Parameter*)> observer){
		this->observers.push_back(observer);
	}

	virtual Gtk::Widget* createParameterWidget() = 0;
protected:
	void fireChangeEvent(){
		for(auto& o : observers){
			o(this);
		}
	}
private:
	std::list<std::function<void(Parameter*)>> observers;
};

template<typename...> struct ParameterWidget;

template<typename T>
struct TypedParameter : Parameter{
	TypedParameter(StaticOutput<T>& correspondingOutput,std::string name)
		:Parameter(name),
		 output(correspondingOutput){}

	virtual bool setValueFromString(std::string s){
		try{
			T val = fromString<T>(s);
			this->setValue(val);
			return true;
		}catch(std::exception& e){
			std::cout << e.what() << std::endl;
			return false;
		}
	}

	virtual std::string getValueAsString() const {
		return toString(this->getValue());
	}

	void setValue(const T& val){
		output.setValue(val);
		this->fireChangeEvent();
	}

	bool hasValue() const{
		return output.hasValue();
	}

	const T& getValue() const{
		return output.getValue();
	}

	Gtk::Widget* createParameterWidget(){
		return Gtk::manage(new ParameterWidget<T>(*this));
	}
protected:
	StaticOutput<T>& output;
};

struct ParamPack{
	ParamPack(std::string name):name(name){}

	Parameter& getParam(const std::string& name){
		return *paramMap.at(name);
	}


	bool hasParam(const std::string& name) const {
		return paramMap.count(name);
	}

	auto begin(){
		return namedParams.begin();
	}

	auto end(){
		return namedParams.end();
	}

	const std::string name;
protected:
	std::vector<std::unique_ptr<Parameter>> namedParams;
	std::map<std::string,Parameter*> paramMap;

	template<typename T> void addParam(const std::string& name,StaticOutput<T>& output){
		this->namedParams.emplace_back(std::make_unique<TypedParameter<T>>(output,name));
		this->paramMap.insert({name,this->namedParams.back().get()});
	}
};

template<typename... Types>
struct UIParameterAction : ParameterAction<Types...>, ParamPack{
	template<typename... Names> UIParameterAction(std::string paramPackName,Names... names)
		:ParamPack(paramPackName){
		std::vector<std::string> nameList{names...};
		if(sizeof...(Types) == 1 && nameList.empty()){
			nameList.push_back(paramPackName);
		}
		this->template addParams(nameList,std::index_sequence_for<Types...>());
	}

	std::string getName() const{
		return name;
	}

	template<size_t N> TypedParameter<NthType<N,Types...>>& getParam(){
		return static_cast<TypedParameter<NthType<N,Types...>>&>(*this->namedParams.at(N));
	}

	const std::vector<std::unique_ptr<Parameter>>& getParams(){
		return this->namedParams;
	}
protected:
	template<size_t... I> void addParams(const std::vector<std::string>& names,std::index_sequence<I...>){
		variadicForEach(this->template addParam(names.at(I),this->template getOutput<I>()));
	}
};

#include "ParameterWidgets.h"
