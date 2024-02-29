#pragma once

class Clock {
    int framesToCountdown;
    int framesRemaining;
    int FPS;
    bool canCount;
    bool isCounting;

public:
    Clock(int seconds, int FPS);
    void Reset(int seconds);
    void Update();
    bool IsTimeUp();
    int GetTimeRemaining();
    void StartCounting();
    void StopCounting();
    bool GetIsCounting();
};
