#pragma once
#include "../../Utility/Utils.h"
#include "../Buffer.h"
#include <vector>
#include <memory>
#include <cstring>

/*
 * CPUBuffer.h
 *
 *  Created on: Mar 27, 2016
 *      Author: tly
 */

template<typename T> struct CPUBuffer : Buffer<T>{
	CPUBuffer(std::size_t elemCount)
	  : Buffer<T>(elemCount),
		data(std::make_shared<std::vector<T>>(elemCount)){}

	T* getDataPointer(){ return data->data(); }
	const T* getDataPointer() const{ return data->data(); }

	void copyToBuffer(std::vector<T>& buffer) const {
		buffer.resize(this->elemCount);
		buffer.insert(buffer.begin(),data->begin(),data->end());
	}

	void copyFromBuffer(const std::vector<T>& buffer,size_t offset, size_t n){
		assertOrThrow(buffer.size() >= offset + n);
		assertOrThrow(this->data->size() >= n);
		std::memcpy(this->getDataPointer(), buffer.data() + offset, n * sizeof(T));
	}

	std::vector<T>& getDataBuffer(){
		return *data;
	}

	std::vector<T>& getDataBuffer() const{
		return *data;
	}

	T& at(size_t x){
		return this->data->at(x);
	}

	const T& at(size_t x) const {
		return this->data->at(x);
	}
protected:
	std::shared_ptr<std::vector<T>> data;
};


