#pragma once
#include <filesystem>  // for path
class World;
struct ServiceContext;

class EditorFileExplorerPanel{
public:
	void render(World* world, ServiceContext* service);
private:
	std::filesystem::path dataPath{"data"};
	std::filesystem::path currentDirectory{dataPath};
	float padding{16.};
	float thumbnailSize{64.};
};
