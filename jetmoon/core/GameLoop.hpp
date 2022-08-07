#pragma once

class WorldManager;
struct ServiceContext;

void preciseSleep(double seconds);

class GameLoop{
public:
		void run(ServiceContext* serviceContext, WorldManager* worldManager);
};
