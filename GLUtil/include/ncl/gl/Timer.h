//
//  Timer.h
//  opengl
//
//  Created by Josiah Ebhomenye on 04/12/2016.
//  Copyright (c) 2016 Josiah Ebhomenye. All rights reserved.
//

#ifndef opengl_Timer_h
#define opengl_Timer_h

#include <chrono>
#include <cmath>
#include <cstring>

using Clock = std::chrono::steady_clock;

static const int MAX_SAMPLE_COUNT = 50;

class Timer{
private:
    static Timer* timer;
    Clock::time_point currentTime;
	float frameTimes[MAX_SAMPLE_COUNT];
	float elapsedTime = 0.0f;
	float totalElapsedTime = 0.0f;
	int sampleCount = 0;
    
public:
    float lastFrameTime = 0.0f;

	float timeSinceStart() {
		return totalElapsedTime / 1000.f;
	}
	
    
private:
    Timer(): currentTime{Clock::now()}{};
    
    
public:
    static void start(){
        timer = new Timer;
    }
    
    static void stop(){
        delete timer;
    }
    
    static Timer& get(){
        return *timer;
    }
    
    void update(){
        using namespace std::chrono;
        auto diff = Clock::now() - currentTime;
		long elapsedTimeMillis = duration_cast<milliseconds>(diff).count();
		totalElapsedTime += elapsedTimeMillis;
		elapsedTime = elapsedTimeMillis / 1000.0f;
		currentTime += diff;

		if (fabsf(elapsedTime - lastFrameTime) < 1.0f) {
			std::memmove(&frameTimes[1], frameTimes, sizeof(frameTimes) - sizeof(frameTimes[0]));
			frameTimes[0] = elapsedTime;

			if (sampleCount < MAX_SAMPLE_COUNT) {
				++sampleCount;
			}
		}
		elapsedTime = 0;

		for (int i = 0; i < sampleCount; i++) {
			lastFrameTime += frameTimes[i];
		}

		if (sampleCount > 0) {
			lastFrameTime /= sampleCount;
		}
		
        
    }

	float now() {
		return totalElapsedTime;
	}
};

Timer* Timer::timer;

#endif
