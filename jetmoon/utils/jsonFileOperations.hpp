#pragma once

#include "utils/json.hpp"                  
#include <filesystem>                        
#include <fstream>                          
#include <thread>
#include <future>

template <class T>
void saveJsonFile(std::string pathStr, const T& data){
	nlohmann::json j(data);
	std::string backupPath = pathStr + ".backup";
	if(std::filesystem::exists(pathStr)){
		std::filesystem::rename(pathStr, backupPath);
	}
	std::string serializedData = j.dump(2);
	std::ofstream out(pathStr);
	out << serializedData;
	out.close();
	if(std::filesystem::exists(backupPath)) std::filesystem::remove(backupPath);
}


template <class T>
auto saveJsonFileAsync(std::string pathStr, const T& data){
	nlohmann::json j(data);
	auto future = std::async(std::launch::async, [pathStr = std::move(pathStr), j = std::move(j)]{
		std::string backupPath = pathStr + ".backup";
		if(std::filesystem::exists(pathStr)){
			std::filesystem::rename(pathStr, backupPath);
		}
		std::string serializedData = j.dump(2);
		std::ofstream out(pathStr);
		out << serializedData;
		out.close();
		if(std::filesystem::exists(backupPath)) std::filesystem::remove(backupPath);
	});
	return future;
}


template <class T>
T loadJsonFile(std::string pathStr){
	namespace fs = std::filesystem;
	std::string backupPath = pathStr + ".backup";
	if(std::filesystem::exists(backupPath)){
		if(std::filesystem::exists(pathStr)) std::remove(pathStr.c_str());
		std::filesystem::rename(backupPath, pathStr);
	}
	if(!std::filesystem::exists(pathStr)){
		saveJsonFile<T>(pathStr, {});
		return {};
	}
	fs::path file{pathStr};
	std::ifstream t(file.string());
	std::string jsonStr((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());
	auto j = nlohmann::json::parse(jsonStr);
	auto data = j.get<T>();
	t.close();
	return data;
}

