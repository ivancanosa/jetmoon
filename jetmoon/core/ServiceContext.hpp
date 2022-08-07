#pragma once

//#include "services/SteamService.hpp"
#include <memory>

class ImageLoader;
class EventService;
class Config;
class RenderContext;
class AnimationDefinitionService;
class ShaderLoader;
class Renderer2D;
class AudioService;
class ImguiService;

struct WorldManagerDS;
struct EditorConfig;
struct EngineStatistics;
struct EditorDS;
struct ConfigDS;

struct ServiceContext{
	std::shared_ptr<WorldManagerDS> worldManagerDS{};
	std::shared_ptr<EventService> eventService{};
	std::shared_ptr<ConfigDS> configDS{};
	std::shared_ptr<RenderContext> renderContext{};
	std::shared_ptr<ImageLoader> imageLoader{};
	std::shared_ptr<ShaderLoader> shaderLoader{};
	std::shared_ptr<AnimationDefinitionService> animationDefinitionService{};
	std::shared_ptr<Renderer2D> renderer2D{};
//	std::shared_ptr<SteamService> steamService{};
	std::shared_ptr<ImguiService> imguiService{};
	std::shared_ptr<AudioService> audioService{};

	std::shared_ptr<EngineStatistics> engineStatistics{};
	std::shared_ptr<EditorDS> editorDS{};

	void hotReload();
};
