#include "core/GameLoop.hpp"
#include <math.h>                                 // for sqrt
#include <stdint.h>                               // for uint32_t, int64_t
#include <memory>
#include <chrono>                                 // for operator-, microsec...
#include <thread>                                 // for sleep_for
#include <type_traits>                            // for enable_if<>::type
#include "Time.hpp"                               // for Time, Time::deltaTime
#include "World.hpp"                       
#include "WorldManager.hpp"                       // for WorldManager
#include "commonStructures/ConfigDS.hpp"          // for ConfigDS, GraphicsC...
#include "commonStructures/EngineStatistics.hpp"  // for EngineStatistics
#include "core/ServiceContext.hpp"                // for ServiceContext
#include "services/EventService.hpp"              // for EventService

void preciseSleep(double seconds) {
    using namespace std;
    using namespace std::chrono;

    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds > estimate) {
        auto start = high_resolution_clock::now();
        this_thread::sleep_for(milliseconds(1));
        auto end = high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2   += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    // spin lock
    auto start = high_resolution_clock::now();
    while ((high_resolution_clock::now() - start).count() / 1e9 < seconds);
}
void GameLoop::run(ServiceContext* serviceContext, WorldManager* worldManager){
	auto world = worldManager->update(serviceContext, serviceContext->worldManagerDS.get());
	auto begin = std::chrono::high_resolution_clock::now();
	auto end = std::chrono::high_resolution_clock::now();
	auto eventService = serviceContext->eventService.get();
	auto* config = serviceContext->configDS.get();
	Time::deltaTime = 16000;
	uint32_t microsecondsPerFrame = 16000;
	uint32_t updateTime = 1000;
	uint32_t sleepTime = 1000;
	while(!eventService->isExit){
		serviceContext->engineStatistics->clear();
		serviceContext->engineStatistics->graphicsStatistics.fps = 1000000./(float(Time::deltaTime));
		world = worldManager->update(serviceContext, serviceContext->worldManagerDS.get());
		//std::cout << "Fps: " << 1000000./Time::deltaTime << std::endl;
		//std::cout << serviceContext->worldManagerDS.actualWorld << std::endl;

		begin = std::chrono::high_resolution_clock::now();
		eventService->update();
		world->update(serviceContext, serviceContext->engineStatistics.get());
		end = std::chrono::high_resolution_clock::now();

		updateTime = std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		Time::deltaTime = updateTime;
		Time::fromLaunch += updateTime;

		microsecondsPerFrame = 1000000/config->graphicsOptions.frameLockCount;
		if(updateTime < microsecondsPerFrame){
			sleepTime = microsecondsPerFrame - updateTime;
		}else{
			continue;
		}
		if(!config->graphicsOptions.vsync && config->graphicsOptions.frameLock){
			begin = std::chrono::high_resolution_clock::now();
			//std::this_thread::sleep_for(std::chrono::microseconds(sleepTime));
			preciseSleep(sleepTime*0.000001);
			end = std::chrono::high_resolution_clock::now();
			Time::deltaTime += std::chrono::duration_cast<std::chrono::microseconds>(end - begin).count();
		}
	}
}
