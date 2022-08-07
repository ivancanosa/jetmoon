#pragma once

#include <filesystem>
#include <string>

#include "steam/steam_api.h"
#include "jetmoon.h"
#include "utils/jsonFileOperations.hpp"
#include "InputEnum.hpp"

#include "WorldCreator.hpp"



class Game{
public:
	Game(){ }

	void init(){

		serviceContext.imguiService = std::make_shared<ImguiService>();
		serviceContext.audioService = std::make_shared<AudioService>();
		serviceContext.worldManagerDS = std::make_shared<WorldManagerDS>();
		serviceContext.eventService = std::make_shared<EventService>();
		serviceContext.renderContext = std::make_shared<RenderContext>();
		serviceContext.imageLoader = std::make_shared<ImageLoader>();
		serviceContext.shaderLoader = std::make_shared<ShaderLoader>();
		serviceContext.animationDefinitionService = std::make_shared<AnimationDefinitionService>();
		serviceContext.renderer2D = std::make_shared<Renderer2D>();
		serviceContext.editorDS = std::make_shared<EditorDS>();
		serviceContext.configDS = std::make_shared<ConfigDS>();
		serviceContext.engineStatistics = std::make_shared<EngineStatistics>();

		*serviceContext.configDS = loadJsonFile<ConfigDS>("configDS.json");

		serviceContext.worldManagerDS->createWorldLambda = createPlayableWorld;
#ifndef __RELEASE__
		auto world = serviceContext.worldManagerDS->createWorldLambda("");
		serviceContext.worldManagerDS->actualWorld = "editor";
		worldManager.addWorld("editor", world, serviceContext.worldManagerDS.get()); 
#endif
#ifdef __RELEASE__
		{
			namespace fs = std::filesystem;
			std::string dir = "data/scenes";
			assert(fs::is_directory(dir) && "Directory data/scenes does not exists");

			const fs::path pathToShow{dir};

			for(auto iterEntry = fs::recursive_directory_iterator(pathToShow);
					 iterEntry != fs::recursive_directory_iterator(); 
					 ++iterEntry ) {
				const auto filenameStr = iterEntry->path().stem().string();
				if(!iterEntry->is_directory()){
					assert((serviceContext.worldManagerDS->addWorld.find(filenameStr) == serviceContext.worldManagerDS->addWorld.end()) && "Scene duplicated");
					std::string extension = iterEntry->path().extension().string();
					if(extension == ".scene"){
						worldManager.addWorld(filenameStr, serviceContext.worldManagerDS->createWorldLambda(iterEntry->path().string()), serviceContext.worldManagerDS.get());
					}
				}
			}
			serviceContext.worldManagerDS->actualWorld = serviceContext.configDS->entryWorld;
		}

#endif

		RenderContext::WindowWidth = serviceContext.configDS->graphicsOptions.resolutionWidth;
		RenderContext::WindowHeight = serviceContext.configDS->graphicsOptions.resolutionHeight;
		serviceContext.renderContext->init();
		serviceContext.eventService->init(serviceContext.renderContext->window);
		serviceContext.shaderLoader->init("data/shaders/shadersDefinition.json");

		serviceContext.imageLoader->addDir("data/images");
		serviceContext.imageLoader->addDir("data/tilesets");
		serviceContext.imageLoader->addDir("data/icons");
		serviceContext.imageLoader->addImage("data/errorAssets/missingTexture");
		serviceContext.imageLoader->setDefaultImage("missingTexture");

		serviceContext.animationDefinitionService->loadDir("data/spriteAnimations");

		serviceContext.renderer2D->init(*serviceContext.shaderLoader.get(), &serviceContext.engineStatistics->graphicsStatistics);

//		serviceContext.steamService.init();
		serviceContext.editorDS->disabledTimelines.push_back("update");
		serviceContext.eventService->semanticInput.mapBool(Input::Jump, InputEnum::Space);
		serviceContext.eventService->semanticInput.mapBool(Input::Fire1, InputEnum::Left_Click);
		serviceContext.eventService->semanticInput.mapBool(Input::Fire2, InputEnum::Right_Click);
		serviceContext.eventService->semanticInput.mapBool(Input::Up,    InputEnum::W);
		serviceContext.eventService->semanticInput.mapBool(Input::Right, InputEnum::D);
		serviceContext.eventService->semanticInput.mapBool(Input::Down,  InputEnum::S);
		serviceContext.eventService->semanticInput.mapBool(Input::Left,  InputEnum::A);

		serviceContext.eventService->semanticInput.mapBool(Input::Dx,  InputEnum::Mouse_X);
		serviceContext.eventService->semanticInput.mapBool(Input::Dy,  InputEnum::Mouse_Y);


		serviceContext.imguiService->addFontToLoad("openSans", "data/fonts/openSans/static/OpenSans/OpenSans-Medium.ttf");
		serviceContext.imguiService->init(serviceContext.renderContext->window);
		serviceContext.imguiService->setDefaultFont("openSans", 18);

		serviceContext.audioService->init();
		serviceContext.audioService->loadSourceFromDir("data/sounds");
		serviceContext.audioService->setAudioConfig(&serviceContext.configDS->audioConfig);
	}

	void run(){
		gameLoop.run(&serviceContext, &worldManager); 
		//TODO: Unload all world here
	}

private:
	GameLoop gameLoop{};
	ServiceContext serviceContext{};
	WorldManager worldManager{};
};
