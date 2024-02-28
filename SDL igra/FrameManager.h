#pragma once
#include <SDL.h>

class FrameManager {
	Uint32 frameStart;
	int FPS;
	int frameDelay;

public:
	FrameManager(int FPS);

	void StartFrame();

	void EndFrame();
};