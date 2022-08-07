#pragma once

#include "steam/steam_api.h"

class SteamService{
public:
	bool isSteamLoaded{false};

	void init(){
		isSteamLoaded = SteamAPI_Init();
	}

	~SteamService(){
		if(isSteamLoaded){
			SteamAPI_Shutdown();
		}
	}
};
