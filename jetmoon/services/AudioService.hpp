#pragma once
#include <filesystem>
#include <assert.h>
#include <vector>
#include <unordered_map>
#include <array>

#include "utils/miniaudio.h"
#include "commonStructures/ConfigDS.hpp"
#include "core/Time.hpp"
#include "core/definitions.hpp"

#include <iostream>

class AudioService{
public:
	void init(){
		configResource = ma_resource_manager_config_init();
		auto result = ma_resource_manager_init(&configResource, &resourceManager);
		assert(result == MA_SUCCESS && "Failed to initialize the resource manager.");

		engineConfig = ma_engine_config_init();
		engineConfig.pResourceManager = &resourceManager;

		result = ma_engine_init(&engineConfig, &engine);
		assert(result == MA_SUCCESS && "Failed initializing the sound engine");

		result = ma_sound_group_init(&engine, 0, NULL, &soundGroups[AudioGroup::MasterGroup]);
		result = ma_sound_group_init(&engine, 0, &soundGroups[AudioGroup::MasterGroup], &soundGroups[AudioGroup::SfxGroup]);
		result = ma_sound_group_init(&engine, 0, &soundGroups[AudioGroup::MasterGroup], &soundGroups[AudioGroup::BgmGroup]);
		result = ma_sound_group_init(&engine, 0, &soundGroups[AudioGroup::MasterGroup], &soundGroups[AudioGroup::VoiceGroup]);
	}

	void setAudioConfig(AudioConfig* audioConfig){
		this->audioConfig = audioConfig;
		this->updateAudioConfig();
	}

	void updateAudioConfig(){
		ma_engine_set_time(&engine, Time::fromLaunch);
		for(int i=0; i<audioConfig->audioGroupVector.size(); i++){
			auto[group, volume] = audioConfig->audioGroupVector[i];
			ma_sound_group_set_volume(&soundGroups[group], volume);
		}
	}

	void setListenerPosition(float x, float y){
		ma_engine_listener_set_position(&engine, 0, x, y, 0.);
	}

	void setGroupVolume(AudioGroup group, float volume){
		ma_sound_group_set_volume(&soundGroups[group], volume);
	}

	void loadSourceFromFile(std::string id, std::string path){
		int flags = 0;
		auto result = ma_resource_manager_data_source_init(&resourceManager, path.c_str(), flags, nullptr, &sourcesMap[id]);
		idToPathMap[id] = path;
		assert(result == MA_SUCCESS && "Failed load sound from file");
	}

	void loadSourceFromDir(std::string dir){
		namespace fs = std::filesystem;

		assert(fs::is_directory(dir) && "Directory does not exists");

		const fs::path pathToShow{dir};


		for(auto iterEntry = fs::recursive_directory_iterator(pathToShow);
				iterEntry != fs::recursive_directory_iterator(); 
				++iterEntry ) {
			const auto filenameStr = iterEntry->path().stem().string();
			if(!iterEntry->is_directory()){
				assert((sourcesMap.find(filenameStr) == sourcesMap.end()) && "Sound Id dupplicate");
				loadSourceFromFile(filenameStr, iterEntry->path().string());
			}
		}
	}

	void playSound(std::string id, ma_sound& sound, AudioGroup audioGroup, int flags=0){
		ma_sound_init_from_file(&engine, idToPathMap[id].c_str(), flags, &soundGroups[audioGroup], NULL, &sound);
		ma_sound_start(&sound);
	}

	void playSoundAtPoint(std::string id, AudioGroup audioGroup, Vec2 point, int flags=MA_SOUND_FLAG_ASYNC){
		ma_sound* sound = new ma_sound();
		ma_sound_init_from_file(&engine, idToPathMap[id].c_str(), flags, &soundGroups[audioGroup], NULL, sound);
		ma_sound_set_position(sound, point.x, point.y, 0.);
		ma_sound_start(sound);
		detachedSounds.insert({detachedSoundStep, sound} );
		detachedSoundStep += 1;
	}

	void processDetachedSounds(){
		static std::vector<unsigned int> deletePos{};
		deletePos.clear();
		for(auto&[s, sound]: detachedSounds){
			if(!ma_sound_is_playing(sound)){
				deletePos.push_back(s);
			}
		}
		for(auto pos: deletePos){
			stopSound(*detachedSounds[pos]);
			delete detachedSounds[pos];
			detachedSounds.erase(pos);
		}
	}

	void deleteDetachedSounds(){
		for(auto&[s, sound]: detachedSounds){
			stopSound(*sound);
			delete sound;
		}
		detachedSounds.clear();
	}

	void stopSound(ma_sound& sound){
		ma_sound_seek_to_pcm_frame(&sound, 0);
		ma_sound_stop(&sound);
		ma_sound_uninit(&sound);
	}

	~AudioService(){
		ma_engine_uninit(&engine);
	}

	ma_resource_manager_config configResource;
	ma_resource_manager resourceManager;

	ma_engine engine;
	ma_engine_config engineConfig;
	std::unordered_map<std::string, ma_resource_manager_data_source> sourcesMap;
	std::unordered_map<std::string, std::string> idToPathMap;

	std::vector<ma_sound> playingSounds{};

	std::array<ma_sound_group, 4> soundGroups;
	AudioConfig* audioConfig{nullptr};

	std::unordered_map<unsigned int, ma_sound*> detachedSounds{};
	unsigned int detachedSoundStep{0};
};
