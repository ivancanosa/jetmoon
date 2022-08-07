#pragma once

#include <filesystem>
#include <iostream>
#include <memory>
#include <fstream>
#include <streambuf>
#include <assert.h>

#include <string>
#include <unordered_map>

#include "utils/json.hpp"


template <class T>
std::unordered_map<std::string, T> loadMapJson(std::string dir){
	std::unordered_map<std::string, T> masterMap;
	std::unordered_map<std::string, T> auxMap;

	namespace fs = std::filesystem;
	assert(fs::is_directory(dir) && "Directory does not exists");
	const fs::path pathToShow{dir};

	for(auto iterEntry = fs::recursive_directory_iterator(pathToShow);
			 iterEntry != fs::recursive_directory_iterator(); 
			 ++iterEntry ) {
		const auto filenameStr = iterEntry->path().stem().string();
		if(!iterEntry->is_directory()){
			std::ifstream t(iterEntry->path().string());
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

			auto j = nlohmann::json::parse(str);
			auxMap = j.get<std::unordered_map<std::string, T>>();
			for(auto const& [key, data] : auxMap){
				assert(masterMap.find(key) != auxMap.end() && "Duplicate key in load map json: " + key +", in directory: " + dir);
				masterMap.insert( {key, data} );
			}
		}
	}

	return std::move(masterMap);
}

//This loads a vector json and extracts a key from each element with a lambda 
template <typename T, typename F>
std::unordered_map<std::string, T> loadVectorJson(std::string dir, F extractKey){
	std::unordered_map<std::string, T> masterMap;
	std::vector<T> auxMap;

	namespace fs = std::filesystem;
	assert(fs::is_directory(dir) && "Directory does not exists");
	const fs::path pathToShow{dir};

	for(auto iterEntry = fs::recursive_directory_iterator(pathToShow);
			 iterEntry != fs::recursive_directory_iterator(); 
			 ++iterEntry ) {
		const auto filenameStr = iterEntry->path().stem().string();
		if(!iterEntry->is_directory()){
			std::ifstream t(iterEntry->path().string());
			std::string str((std::istreambuf_iterator<char>(t)), std::istreambuf_iterator<char>());

			auto j = nlohmann::json::parse(str);
			try{
				auxMap = j.get<std::vector<T>>();
			}catch(...){
				auxMap = {};
				auxMap.emplace_back(j.get<T>());
			}
			for(auto const& data : auxMap){
				std::string key = extractKey(data, filenameStr);
//				assert(masterMap.find(key) != masterMap.end() && "Duplicate key in load vector json: " + key +", in directory: " + dir);
				masterMap.insert( {key, data} );
			}
		}
	}

	return std::move(masterMap);
}
