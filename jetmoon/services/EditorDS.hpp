#include <vector>
#include <string>
#include <memory>
#include "opengl/Texture.hpp"
#include "commonStructures/EditorConfig.hpp"

struct EditorDS{
	bool isPlaying{false}, renderGrid{true}, reload{false};
	std::vector<std::string> disabledTimelines{};
	EditorConfig editorConfig{};
	std::shared_ptr<Texture> entityMapTexture{nullptr};
};

