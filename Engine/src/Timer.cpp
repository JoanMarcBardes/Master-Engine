#include "Timer.h"

Timer::Timer()
{
	Start();
}

void Timer::Start()
{
	isRuning = true;
	start = SDL_GetTicks();
}

void Timer::Stop()
{
	isRuning = false;
	stop = SDL_GetTicks();
}

void Timer::Continue()
{
	if (isRuning == false)
	{
		isRuning = true;
		start = SDL_GetTicks() - (stop - start);
	}
}

unsigned Timer::GetTimer() const
{
	if (isRuning == true)
	{
		return SDL_GetTicks() - start;
	}
	else
	{
		return stop - start;
	}
}

float Timer::GetTimerSec() const
{
	return (float)(SDL_GetTicks() - start) / 1000.0f;
}

