#pragma once

#include <vector>
#include <unordered_set>
#include <unordered_map>
#include <string>
#include <memory>


class World;

//TODO: Async functionality
struct WorldManagerDS{
	std::string actualWorld{""};
	std::unordered_map<std::string, std::shared_ptr<World>> addWorld{};
	std::vector<std::string> unloadWorlds{};
	std::vector<std::string> loadWorlds{};
	std::vector<std::string> deleteWorlds{};

	bool async{false};
	bool finished{true};

	std::unordered_set<std::string> legalWorlds{};
	std::unordered_set<std::string> loadedWorlds{};

	std::function<std::shared_ptr<World>(std::string)> createWorldLambda{};
};
