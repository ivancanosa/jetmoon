#include "systems/AudioSystem.hpp"
#include <iostream>                     // for string
#include <set>                          // for set
#include "commonStructures/Camera.hpp"  // for Camera
#include "components/AudioSource.hpp"   // for AudioSource
#include "components/Transform.hpp"     // for Transform
#include "core/ServiceContext.hpp"      // for ServiceContext
#include "core/World.hpp"               // for World
#include "core/WorldContext.hpp"        // for WorldContext
#include "services/AudioService.hpp"    // for AudioService

ISystem* AudioSystem::clone(){
	auto audioSystem = new AudioSystem();
	audioSystem->audioService = audioService;
	return audioSystem;
}

std::string_view AudioSystem::getName(){
	static std::string str{"AudioSystem"};
	return str;
}


void AudioSystem::removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){ 
	if(ma_sound_is_playing(&soundsArray[entity]))
		serviceContext->audioService->stopSound(soundsArray[entity]);
}


void AudioSystem::init(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	this->audioService = serviceContext->audioService.get();
}

void AudioSystem::update(World* world, WorldContext* worldContext, ServiceContext* serviceContext){
	serviceContext->audioService->updateAudioConfig();
	Entity activeCamera = worldContext->camera.activeCamera;
	if(activeCamera != NullEntity){
		auto tr = world->reduceComponent<Transform>(activeCamera);
		serviceContext->audioService->setListenerPosition(tr.position.x, tr.position.y);
	}

	for(auto&& entity: this->mEntities){
		auto& audioSource = world->getComponent<AudioSource>(entity);
		if(audioSource.reset && audioSource.isPlaying){
			serviceContext->audioService->stopSound(soundsArray[entity]);
			audioSource.isPlaying = false;
			audioSource.reset = false;
		}
		if(audioSource.play && !audioSource.isPlaying){
			int flags = MA_SOUND_FLAG_ASYNC;
			if(!audioSource.isPositional){
				flags |= MA_SOUND_FLAG_NO_SPATIALIZATION;
			}
			serviceContext->audioService->playSound(audioSource.audioId, soundsArray[entity], audioSource.audioGroup, flags);
			audioSource.isPlaying = true;
			ma_sound_set_volume(&soundsArray[entity], audioSource.volume);
			ma_sound_set_looping(&soundsArray[entity], audioSource.playInLoop);
			ma_sound_set_pitch(&soundsArray[entity], audioSource.pitch);
		}
		else if(!audioSource.play && audioSource.isPlaying){
			serviceContext->audioService->stopSound(soundsArray[entity]);
			audioSource.isPlaying = false;
		}else if(audioSource.isPlaying){
			bool isPlaying = ma_sound_is_playing(&soundsArray[entity]);
			if(!isPlaying){
				serviceContext->audioService->stopSound(soundsArray[entity]);
				audioSource.isPlaying = false;
				audioSource.play = false;
			}else if(audioSource.isPositional){
				auto tr = fromMatrix(world->reduceTransformMatrix(entity));
				ma_sound_set_position(&soundsArray[entity], tr.position.x, tr.position.y, 0.);
			}
		}
	}
	audioService->processDetachedSounds();
}

AudioSystem::~AudioSystem(){
	for(auto&& entity: this->mEntities){
		if(ma_sound_is_playing(&soundsArray[entity]))
			audioService->stopSound(soundsArray[entity]);
	}
	audioService->deleteDetachedSounds();
}


