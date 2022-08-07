#include "definitions.hpp"
#include <stdlib.h>                             // for rand
#include <cstdint>                              // for uint64_t
#include <random>                               // for mt19937_64, random_de...

UUID createUUID(){
	static std::random_device s_RandomDevice;
	static std::mt19937_64 s_Engine(s_RandomDevice());
	static std::uniform_int_distribution<uint64_t> s_UniformDistribution;
	return s_UniformDistribution(s_Engine);
}




