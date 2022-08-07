#include "WorldManager.hpp"
#include <map>                          // for map
#include <type_traits>                  // for remove_extent_t
#include <unordered_map>                // for unordered_map, __hash_map_ite...
#include <unordered_set>                // for unordered_set, unordered_set<...
#include <vector>                       // for vector
#include "services/WorldManagerDS.hpp"  // for WorldManagerDS
#include "World.hpp"  
struct ServiceContext;

void WorldManager::addWorld(std::string id, std::shared_ptr<World> world, WorldManagerDS* worldManager){
	worldMap.insert( {id, world} );
	worldManager->legalWorlds.insert(id);
}

std::shared_ptr<World> WorldManager::update(ServiceContext* serviceContext, WorldManagerDS* worldManager){
	for(auto&[id, ptr]: worldManager->addWorld){
		worldMap.insert( {id, ptr} );
		worldManager->legalWorlds.insert(id);
	}

	for(auto& id: worldManager->unloadWorlds){
		worldMap[id]->unload();
		worldManager->loadedWorlds.erase(id);
	}

	for(auto& id: worldManager->deleteWorlds){
		worldMap[id]->unload();
		worldManager->loadedWorlds.erase(id);
		worldManager->legalWorlds.erase(id);
		worldMap.erase(id);
	}

	for(auto& id: worldManager->loadWorlds){
		worldMap[id]->load(serviceContext);
		worldManager->loadedWorlds.insert(id);
	}

	worldManager->addWorld.clear();
	worldManager->unloadWorlds.clear();
	worldManager->deleteWorlds.clear();
	worldManager->loadWorlds.clear();

	if(actualWorldID != worldManager->actualWorld){
		actualWorldID = worldManager->actualWorld;
		if(worldManager->loadedWorlds.find(actualWorldID) == worldManager->loadedWorlds.end()){
			worldMap[actualWorldID]->load(serviceContext);
			worldManager->loadedWorlds.insert(actualWorldID);
		}
	}
	return worldMap[actualWorldID];
}

