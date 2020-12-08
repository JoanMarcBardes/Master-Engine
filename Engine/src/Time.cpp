#include "Time.h"

float Time::deltaTime = 0.0f;
float Time::scaleTime = 1.0f;
float Time::time = 0.0f;
Timer Time::gameTimer;
bool  Time::paused = false;
bool  Time::running = false;
bool  Time::step = false;

void Time::Start()
{
	running = true;
	gameTimer.Start();
	time = 0;
}

void Time::PreUpdate(float dt)
{
	bool active = running || step;
	deltaTime = active ? dt : 0;
	if (active)
		time = gameTimer.GetTimerSec();
}

void Time::PostUpdate()
{
	step = false;
}

void Time::Play()
{
	running = true;
}

void Time::Pause()
{
	paused = true;
	gameTimer.Stop();
}

void Time::Stop()
{
	running = false;
	gameTimer.Stop();
}

void Time::Continue()
{
	paused = false;
	gameTimer.Continue();
}

void Time::DoStep()
{
	step = true;
}
