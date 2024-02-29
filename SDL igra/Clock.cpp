#include "Clock.h"

Clock::Clock(int seconds, int FPS) {
    this->FPS = FPS;
    Reset(seconds);
}

void Clock::Reset(int seconds) {
    framesToCountdown = seconds * FPS;
    framesRemaining = framesToCountdown;
    canCount = false;
    isCounting = false;
}

void Clock::Update() {
    if (canCount) {
        framesRemaining--;

        if (framesRemaining < 0) {
            StopCounting();
        }
    }
}

bool Clock::IsTimeUp() {
    return framesRemaining <= 0;
}

int Clock::GetTimeRemaining() {
    return framesRemaining / FPS;
}

void Clock::StartCounting() {
    canCount = true;
    isCounting = true;
}

void Clock::StopCounting() {
    canCount = false;
}

bool Clock::GetIsCounting() {
    return isCounting;
}
