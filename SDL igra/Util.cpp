#include "Util.h"

const int Util::windowWidth = 832;
const int Util::windowHeight = 640;
const int Util::FPS = 60;

bool Util::WithinBoundsX(int x, int width) {
    return x >= 0 && x <= windowWidth - width * 2;
}

bool Util::WithinBoundsY(int y, int height) {
    return y >= 0 && y <= windowHeight - height;
}

int Util::ResetX(int x, int width) {
    if (x < 0) {
        return 0;
    }
    else {
        return windowWidth - width * 2;
    }
}

int Util::ResetY(int y, int height) {
    if (y < 0) {
        return 0;
    }
    else {
        return windowHeight - height;
    }
}

int Util::GetRandomX(int width) {
    return rand() % (windowWidth - width * 2);
}

int Util::GetRandomY(int height) {
    return rand() % (windowHeight - height * 2);
}