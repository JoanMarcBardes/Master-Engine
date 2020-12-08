#pragma once
#include "Timer.h"

static class Time
{
public:
	static void Start();
	static void PreUpdate(float dt);
	static void PostUpdate();
	
	static void Play();
	static void Pause();
	static void Stop();
	static void Continue();
	static void DoStep();

public:
	static float deltaTime;
	static float scaleTime;
	static float time;
	static Timer gameTimer;
	static bool paused;
	static bool running;
	static bool step;
};

