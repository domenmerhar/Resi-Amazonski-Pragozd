#include "FrameManager.h"
#include <SDL.h>

FrameManager::FrameManager(int FPS) {
    this->FPS = FPS;
    frameDelay = 1000 / FPS;
    frameStart = SDL_GetTicks();
}

void FrameManager::StartFrame() {
    frameStart = SDL_GetTicks();
}

void FrameManager::EndFrame() {
    int frameTime = SDL_GetTicks() - frameStart;

    if (frameDelay > frameTime) {
        SDL_Delay(frameDelay - frameTime);
    }
}