#pragma once
#include <stdint.h>  // for uint32_t
#include <chrono>    // for steady_clock, steady_clock::time_point

class Stopwatch{
public:
	void start();
	void resume();
	void stop();
	void pause();
	uint32_t getTime();
private:
	uint32_t time{0};
	std::chrono::steady_clock::time_point begin{};
};
