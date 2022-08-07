#pragma once

#include <assert.h>
#include <memory>
#include <unordered_map>

#include "definitions.hpp"
#include "ISystem.hpp"

#include "utils/type_name.hpp"

class World;
struct ServiceContext;
struct WorldContext;


/*	std::unordered_map<const char*, Signature> mSignatures{};

	// Map from system type string pointer to a system pointer
	std::unordered_map<const char*, std::shared_ptr<ISystem>> mSystems{};*/

class SystemManager {
public:
	SystemManager(){}

	SystemManager(const SystemManager& systemManager){
		for(const auto&[key, value]: systemManager.mSignatures){
			mSignatures.insert( {key, value} );
		}
	}

	template<typename T>
	std::shared_ptr<T> registerSystem() {
		//const char* typeName = typeid(T).name();
		const char* typeName = type_nameStr<T>();

		assert(mSystems.find(typeName) == mSystems.end() && "Registering system more than once.");

		auto system = std::make_shared<T>();
		mSystems.insert({typeName, system});
		return system;
	}

	template<typename T>
	void setSignature(Signature signature) {
		//const char* typeName = typeid(T).name();
		const char* typeName = type_nameStr<T>();

		assert(mSystems.find(typeName) != mSystems.end() && "System used before registered.");

		// Set the signature for this system
		mSignatures.insert({typeName, signature});
	}

	void entityDestroyed(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext) { // Erase a destroyed entity from all system lists
		// mEntities is a set so no check needed
		for (auto const& pair : mSystems) {
			auto const& system = pair.second;

			if (system->mEntities.find(entity) != system->mEntities.end()) {
				system->mEntities.erase(entity);
				system->removedEntity(entity, world, worldContext, serviceContext);
			}
		}
	}

	void entitySignatureChanged(Entity entity, Signature entitySignature, World* world, WorldContext* worldContext, ServiceContext* serviceContext) {
		stepsArray[entity] += 1;
		int currentStep = stepsArray[entity];
		// Notify each system that an entity's signature changed
		for (auto const& pair : mSystems) {
			if(currentStep != stepsArray[entity]) return;
			auto const& type = pair.first;
			auto const& system = pair.second;
			auto const& systemSignature = mSignatures[type];

			// Entity signature matches system signature - insert into set
			if (((entitySignature & systemSignature) == systemSignature) && (system->mEntities.find(entity) == system->mEntities.end()) ) {
				system->mEntities.insert(entity);
				system->addedEntity(entity, world, worldContext, serviceContext);
			}
			// Entity signature does not match system signature - erase from set
			else if (((entitySignature & systemSignature) != systemSignature) && system->mEntities.find(entity) != system->mEntities.end()){
				system->removedEntity(entity, world, worldContext, serviceContext);
				system->mEntities.erase(entity);
			}
		}
	}

	SystemManager* clone(){
		return new SystemManager(*this);
	}

	~SystemManager(){
		mSignatures.clear();
		mSystems.clear();
	}


	// Map from system type string pointer to a system pointer
	std::unordered_map<const char*, std::shared_ptr<ISystem>> mSystems{};
private:
	// Map from system type string pointer to a signature
	std::unordered_map<const char*, Signature> mSignatures{};
	std::array<unsigned char, MAX_ENTITIES> stepsArray{0};

};
