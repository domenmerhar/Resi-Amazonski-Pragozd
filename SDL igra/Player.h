#pragma once

#include "GameObject.h"
#include "Tree.h"
#include "ScoreCounter.h"
#include <SDL.h>
#include <SDL_image.h>
#include <vector>

class Player : public GameObject {
    SDL_Surface* surfaceLeft;
    SDL_Surface* surfaceRight;
    ScoreCounter* scoreCounter;

public:
    using GameObject::GameObject;

    void HandleInput();
    void HandleTreeCollision(std::vector<Tree*> treesInDestruction);
    void Update();

    Player(const char* imagePathRight, const char* imagePathLeft, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible, ScoreCounter* scoreCounter);
};
