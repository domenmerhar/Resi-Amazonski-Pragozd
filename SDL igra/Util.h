#pragma once

#include <cstdlib> 

class Util {
public:
    static const int windowWidth;
    static const int windowHeight;
    static const int FPS;

    static bool WithinBoundsX(int x, int width);
    static bool WithinBoundsY(int y, int height);
    static int ResetX(int x, int width);
    static int ResetY(int y, int height);
    static int GetRandomX(int width);
    static int GetRandomY(int height);
    static const char* IntToCharPointer(int number);
};