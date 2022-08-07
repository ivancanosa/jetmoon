#pragma once
#include <map>                            // for map, __map_iterator, operat...
#include "commonStructures/ConfigDS.hpp"  // for ConfigDS

class ConfigService{
public:
	void registerCallback(void* key, std::function<void(const ConfigDS*)> callback){
		callbackMap.insert( {key, callback} );
	}

	void unregisterCallback(void* key){
		callbackMap.erase(callbackMap.find(key));
	}

	const ConfigDS* getConfig(){
		return &config;
	}

	void setConfig(ConfigDS config){
		this->config = config;
		notify();
	}

private:
	void notify(){
		for(auto&& i: callbackMap){
			i.second(&config);
		}
	}

	ConfigDS config{};
	std::map<void*, std::function<void(const ConfigDS*)>> callbackMap{};
};


