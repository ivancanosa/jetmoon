#pragma once

#include <cstdint>

//deltaTime is in microseconds
//From launch is in microseconds
struct Time{
	static uint32_t deltaTime;
	static uint64_t fromLaunch;
	static unsigned long frameCount;
};

