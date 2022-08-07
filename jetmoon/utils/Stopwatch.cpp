#include "Stopwatch.hpp"
#include <chrono>       // for microseconds, duration_cast, operator-, high_...
#include <type_traits>  // for enable_if<>::type

void Stopwatch::start(){
	time = 0;
	begin = std::chrono::high_resolution_clock::now();
}

void Stopwatch::resume(){
	begin = std::chrono::high_resolution_clock::now();
}

void Stopwatch::stop(){
	time = 0;
}

void Stopwatch::pause(){
	auto end = std::chrono::high_resolution_clock::now();
	time += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
}

uint32_t Stopwatch::getTime(){
	auto end = std::chrono::high_resolution_clock::now();
	return time + std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
}
