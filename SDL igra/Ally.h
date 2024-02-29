#pragma once

#include "GameObject.h"
#include "Util.h"
#include "Tree.h"
#include <vector>
#include <SDL.h>
#include <SDL_image.h>

class Ally : public GameObject {
private:
    int spawnX, spawnY;
    int targetX, targetY;
    SDL_Surface* surfaceLeft, * surfaceRight;

    void GenerateRandomTarget();
    void Init(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height);

public:
    Ally(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height);
    Ally(const char* imagePathRight, const char* imagePathLeft, SDL_Renderer* renderer, int movementSpeed, int width, int height);

    void Reset(int movementSpeed);
    void Move();
    void Hide();
    bool IsInRange(int x, int y, int range);
    bool IsTreeInRange(Tree* tree, int range);
    void SetTarget(int x, int y, int width, int height);
    void HandleTreeCollision(std::vector<Tree*> treesInDestruction);
};
