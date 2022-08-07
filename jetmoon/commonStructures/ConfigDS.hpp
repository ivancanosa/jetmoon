#pragma once
#include <tuple>
#include <vector>
#include <map>
#include <string>
#include "utils/ComponentInspector.hpp"

enum ScreenMode{Windowed, Fullscreen, Borderless, None};
enum ConfigOption{General, Graphics, Input, Gameplay, Audio};
enum AudioGroup{
	MasterGroup,
	SfxGroup,
	BgmGroup,
	VoiceGroup
};
const char* const AudioGroupStr[] {"MasterGroup", "SfxGroup", "BgmGroup", "VoiceGroup"};
const char* const ScreenModeStr[] {"Windowed", "Fullscreen", "Borderless"};

struct GraphicsConfig{
	ScreenMode screenMode{ScreenMode::Borderless};
	bool vsync{true};
	bool frameLock{true};
	int frameLockCount{60};
	float resolutionWidth{1920};
	float resolutionHeight{1080};
	//antialiasing

	static std::map<std::string, std::tuple<int, int>> legalResolutions;
	static std::map<std::string, ScreenMode> legalScreenModes;
};
COMPONENT_INSPECTOR(GraphicsConfig, screenMode, vsync, frameLock, frameLockCount, resolutionWidth, resolutionHeight);

struct DataDirs{
	std::string images{"data/images"};
	std::string spriteAnimations{"data/spriteAnimations"};
	std::string tilesets{"data/tilesets"};
	std::string prefabs{"data/prefabs"};
	std::string shaders{"data/shaders"};
};

COMPONENT_INSPECTOR(DataDirs, images, spriteAnimations, tilesets, prefabs, shaders);

struct AudioConfig{
	std::vector<std::tuple<AudioGroup, float>> audioGroupVector = {
		{AudioGroup::MasterGroup, 1.0f},
		{AudioGroup::SfxGroup, 1.f},
		{AudioGroup::BgmGroup, 1.0f},
		{AudioGroup::VoiceGroup, 1.f},
	};
};
COMPONENT_INSPECTOR(AudioConfig, audioGroupVector);

struct ConfigDS{
	GraphicsConfig graphicsOptions{};
	AudioConfig audioConfig{};
	DataDirs dataDirs{};
	std::string entryWorld{"main"};
};

COMPONENT_INSPECTOR(ConfigDS, graphicsOptions, dataDirs, audioConfig, entryWorld);
