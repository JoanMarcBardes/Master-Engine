#pragma once
#include "SDL.h"

class Timer
{
public:
	Timer();

	void Start();
	void Stop();
	void Continue();

	unsigned GetTimer() const;
	float GetTimerSec() const;

	bool IsRunning() const { return isRuning; };

private:
	bool isRuning = true;
	unsigned start = 0;
	unsigned stop = 0;

};

