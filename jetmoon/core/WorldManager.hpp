#pragma once

#include <memory>
#include <iosfwd>                 // for string
#include <map>                    // for map
#include <string>                 // for operator<
class World;
struct ServiceContext;
struct WorldManagerDS;



class WorldManager{
	public:
		void addWorld(std::string id, std::shared_ptr<World> world, WorldManagerDS* worldManager);
		std::shared_ptr<World> update(ServiceContext* serviceContext, WorldManagerDS* worldManager);

	private:
		std::map<std::string, std::shared_ptr<World>> worldMap;
		std::string actualWorldID{""};
};

