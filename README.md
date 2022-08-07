# Jetmoon

![alt text](https://github.com/ivancanosa/jetmoon/blob/main/gameEngine.png?raw=true)

## This is a work in Progress

## Introduction
Jetmoon is a multi-platform 2D game engine that is based on the Entity-Component-System design. It does not use any scripting language and is programmed in C++, so it is easy to use any external libraries. It uses OpenGL for the graphics, Box2D for the physics and ImGui for the interface of the game engine.

## Platforms:
It has been tested on Windows and Linux. With linux, you have to use "make" to compile, while en Windows you need to execute the bat file. Both have the same arguments to compile.

## Features:
- Pure ECS design
- Grouping and ordering of systems into timelines
- Physics
- Tilemaps support
- Lights
- Post-Processing pipeline
- Particles system
- Pathfinding based on Navigation Mesh
- World serialization

## Requirements:
- Meson
- Clang 13.0+
- libc++

## Compilation
It is necessary to have make, meson, clang, and libc++. These are the following commands:
- make build: Compile the project
- make run: Run the project
- make debug: Run the project in debug mode. There is a stacktrace if a segfault happens.
- make release: Compile the project for the release version
- make runRelease: Run the release compiled version
- make clean: Delete all compiled files

## ECS design:
In the Entity-Component-System design the Entities does not have any data, they are only an identifier. The components are the actual data that are owned by the entities. Each entity can have any subset of the possible components. Lastly, the systems creates the behavior by reading and writing to the components. Additionally, this project has the following concepts:
- World: Class that contains all the ECS data of the actual playing stage. A game can contain multiple worlds, each of them self-contained.
- Timelines: The world organize its systems in timelines. The order of execution of the timelines and the systems inside them are specified by the world. They can be enabled or disabled, so, for example, one could disable the "Render" timeline so nothing is drawn to the screen. This facilitates thing like a "pause" option for the game, by disabling all timelines that updates the state of the world, but leaving the interface and rendering working normally.
- WorldContext: Struct that contains data and services that are shared across all system of the same world.
- ServiceContext: Struct that contains data and services that are shared across all worlds.

## System:
There is an interface called ISystem which all systems derive. It contains the following fields and functions:
- mEntities: std::set\<Entity\> that contains all entities relevant to the system
- addedEntity: Is called when a new entity is created with the components that are relevant to the system
- removedEntity: Is called when a entity has been removed of a component relevant to the system
- init: Is called when the system is initialized
- update: Is called every frame
- getName: Get the name of the system in string format
- clone: Duplicate the system
If you want to create a new system, you need to implement the following interface:
```
class ISystem {
public:
	std::set<Entity> mEntities;
	virtual void addedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){}
	virtual void removedEntity(Entity entity, World* world, WorldContext* worldContext, ServiceContext* serviceContext){}
	virtual void init(World* world, WorldContext* worldContext, ServiceContext* serviceContext) = 0;
	virtual void update(World* world, WorldContext* worldContext, ServiceContext* serviceContext) = 0;
	virtual std::string_view getName(){static std::string str{"defaultSystemName"}; return str;}
	virtual ISystem* clone() = 0;
	virtual ~ISystem() = default;
};
```
## Directory Structure
- jetmoon: Contains all the headers and sources of the game engine
- game: Contains all the headers and sources specific for the game
- subprojects: Other projects that are used and have a meson build file
- data: Contains all the data that will be used by the game. Counts as data thing like languages, images, shaders, tilemaps, fonts, prefabs, scenes...
- builddir: All compiled files
- release: Contains all the compiles files, data and executable of the release version of the game

## Add new Components:
You need to create the header of the component somewhere accessible. The file would contain something like the following:
```
#pragma once
#include "core/definitions.hpp"
#include "utils/ComponentInspector.hpp"

struct MyComponent{
	float myFloatField{0.};
	int myIntegerField{0};
};
COMPONENT_INSPECTOR(MyComponent, myFloatField, myIntegerField);
```
The COMPONENT_INSPECTOR macro allows your struct to be serializeable and accessible from the editor to modify its fields. Currently the component can have all C++ types, all structs with a COMPONENT_INSPECTOR macro, std::variant and containers such as std::vector and std::set.
Also, you need to add your component to the ComponentVariant type in the file "jetmoon/config.h".

## Tilemap:
Jetmoon uses tilemaps exported by the Tiled editor in the format json. All used tilesets must be saved with json format.

## Editor Shortcuts:
- Ctrl+d : Duplicate selected entities
- Supr: Delete selected entities
- Ctrl+U: Ungroup selected entities
- Ctrl+G: Group selected entities
