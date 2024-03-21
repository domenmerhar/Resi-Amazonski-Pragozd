#include "Enemy.h"
#include "Util.h"
#include "Forest.h"

#include <cmath>
#include <SDL.h>


void Enemy::GenerateRandomTarget() {
    targetX = Util::GetRandomX(width);
    targetY = Util::GetRandomY(height);

    targetRow = targetY / 64;
    targetColumn = targetX / 64;
}

Enemy::Enemy(SDL_Renderer* renderer, int movementSpeed, int width, int height, Forest* forest,
    const char* pathImageSmallLeft, const char* pathImageSmallRight, const char* pathImageBigRight, const char* pathImageBigLeft) {
    this->width = width;
    this->height = height;

    surfaceSmallLeft = IMG_Load(pathImageSmallLeft);
    surfaceSmallRight = IMG_Load(pathImageSmallRight);

    surfaceBigLeft = IMG_Load(pathImageBigLeft);
    surfaceBigRight = IMG_Load(pathImageBigRight);

    this->renderer = renderer;

    SDL_Surface* tempSurface = IMG_Load(pathImageSmallRight);
    this->texture = SDL_CreateTextureFromSurface(this->renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    this->forest = forest;

    Reset(movementSpeed);
}

void Enemy::Reset(int movementSpeed) {
    spawnX = x = Util::GetRandomX(width);
    spawnY = y = Util::GetRandomY(height);
    this->movementSpeed = movementSpeed;

    UpdateSourceRectangle();

    GenerateRandomTarget();

    SetBig(false);
    visible = true;

    boundingBox = GetBoundingBox();
}

void Enemy::Move() {
    float dx = targetX - x;
    float dy = targetY - y;

    float magnitude = sqrt(dx * dx + dy * dy);

    if (!forest->CanBeDestroyed(targetRow, targetColumn))
        GenerateRandomTarget();
    else {
        if (magnitude > 0) {
            dx /= magnitude;
            dy /= magnitude;

            x += round(dx * movementSpeed);
            y += round(dy * movementSpeed);
        }
        else {
            forest->StartBurning(targetRow, targetColumn);
            GenerateRandomTarget();
        }

        if (dx > 0 && !isBig) {
            texture = SDL_CreateTextureFromSurface(renderer, surfaceSmallRight);
        }
        else if (dx < 0 && !isBig) {
            texture = SDL_CreateTextureFromSurface(renderer, surfaceSmallLeft);
        }
        else if (dx < 0 && isBig) {
            texture = SDL_CreateTextureFromSurface(renderer, surfaceBigRight);
        }
        else if (dx > 0 && isBig) {
            texture = SDL_CreateTextureFromSurface(renderer, surfaceBigLeft);
        }
    }
}

void Enemy::Hide() {
    visible = false;
    x = spawnX;
    y = spawnY;
}

void Enemy::SetTarget(Tree* tree) {
    targetX = tree->GetX() + tree->getWidth() / 2;
    targetY = tree->GetY() + tree->getHeight() / 2;
}

void Enemy::SetBig(bool toSet) {
    if (isBig == toSet) return;

    isBig = toSet;
}

bool Enemy::GetIsBig() {
    return isBig;
}

void Enemy::HandleCollision(std::vector<Ally*> allies, std::vector<Enemy*> enemies, Player* player, ScoreCounter* scoreCounter) {
    if (!visible) return;

    boundingBox = GetBoundingBox();
    SDL_Rect playerBoundingBox = player->GetBoundingBox();
    bool invinciblePlayer = false;

    for (Ally* ally : allies) {
        bool playerIsCloseToAlly = false;
        if (ally != nullptr && ally->GetIsVisible()) {
            SDL_Rect allyBoundingBox = ally->GetBoundingBox();

            if ((abs(playerBoundingBox.x - allyBoundingBox.x) < 100 && abs(playerBoundingBox.y - allyBoundingBox.y) < 100) && !playerIsCloseToAlly) {
                playerIsCloseToAlly = true;
                invinciblePlayer = true;
            }

            if (SDL_HasIntersection(&allyBoundingBox, &boundingBox)) {
                if (!isBig) Hide();
                else if (isBig) {
                    if (SDL_HasIntersection(&playerBoundingBox, &boundingBox)) {
                        this->Hide();
                        scoreCounter->AddScore(5000);
                    }
                    else if (!playerIsCloseToAlly) {
                        ally->Hide();
                    }
                }
            }
        }
    }

    for (Enemy* enemy : enemies) {
        if (enemy != nullptr && enemy->GetIsVisible() && enemy != this) {
            SDL_Rect enemyBoundingBox = enemy->GetBoundingBox();
            if (SDL_HasIntersection(&enemyBoundingBox, &boundingBox) && isBig == false) {
                enemy->Hide();
                SetBig(true);
            }
        }
    }

    if (!isBig && SDL_HasIntersection(&playerBoundingBox, &boundingBox)) {
        this->Hide();
        scoreCounter->AddScore(1000);
    }

    if (!player->GetIsVisible() || invinciblePlayer) return;

    if (isBig && SDL_HasIntersection(&playerBoundingBox, &boundingBox)) {
        player->Hide();
    }
}
