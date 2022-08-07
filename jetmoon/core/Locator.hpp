#pragma once

#include <memory>

template <typename T>
class Locator{
	public:
		static void setService(std::shared_ptr<T> service);
		static std::shared_ptr<T> getService();

	private:
		static std::shared_ptr<T> service;
};

template <typename T> void Locator<T>::setService(std::shared_ptr<T> service){
	Locator<T>::service = service;
}

template <typename T> std::shared_ptr<T> Locator<T>::getService(){
	return Locator<T>::service;
}

template <typename T> std::shared_ptr<T> Locator<T>::service{nullptr};


