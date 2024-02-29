#pragma once

#include "GameObject.h"
#include "Forest.h"
#include "Ally.h"
#include "Player.h"
#include <vector>

class Enemy : public GameObject {
    int spawnX, spawnY;
    int targetX, targetY;

    int targetRow, targetColumn;

    bool isBig;

    Forest* forest;

    SDL_Rect boundingBox;

    SDL_Surface* surfaceSmallLeft, * surfaceSmallRight, * surfaceBigLeft, * surfaceBigRight;

    void GenerateRandomTarget();

public:
    Enemy(SDL_Renderer* renderer, int movementSpeed, int width, int height, Forest* forest,
        const char* pathImageSmallLeft, const char* pathImageSmallRight, const char* pathImageBigRight, const char* pathImageBigLeft);

    void Reset(int movementSpeed);

    void Move();

    void Hide();

    void SetTarget(Tree* tree);

    void SetBig(bool toSet);

    bool GetIsBig();

    void HandleCollision(std::vector<Ally*> allies, std::vector<Enemy*> enemies, Player* player);
};