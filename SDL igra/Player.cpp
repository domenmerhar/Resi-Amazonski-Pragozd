#include "Player.h"
#include "ScoreCounter.h"

void Player::HandleInput() {
    if (visible) {
        const float diagonalSpeedMultiplier = 0.7071;

        SDL_PumpEvents();

        const Uint8* keys = SDL_GetKeyboardState(NULL);

        float dx = 0, dy = 0;

        if (keys[SDL_SCANCODE_UP] || keys[SDL_SCANCODE_W]) {
            dy -= 1;
        }

        if (keys[SDL_SCANCODE_DOWN] || keys[SDL_SCANCODE_S]) {
            dy += 1;
        }

        if (keys[SDL_SCANCODE_LEFT] || keys[SDL_SCANCODE_A]) {
            dx -= 1;
        }

        if (keys[SDL_SCANCODE_RIGHT] || keys[SDL_SCANCODE_D]) {
            dx += 1;
        }

        if (dx != 0 && dy != 0) {
            dx = dx * diagonalSpeedMultiplier;
            dy = dy * diagonalSpeedMultiplier;
        }

        x += dx * movementSpeed;
        y += dy * movementSpeed;

        if (dx < 0) texture = SDL_CreateTextureFromSurface(renderer, surfaceRight);
        else if (dx > 0) texture = SDL_CreateTextureFromSurface(renderer, surfaceLeft);
    }
}

void Player::HandleTreeCollision(std::vector<Tree*> treesInDestruction) {
    for (Tree* tree : treesInDestruction) {
        if (visible && tree != nullptr) {
            SDL_Rect playerBoundingBox = GetBoundingBox();
            SDL_Rect treeBoundingBox = tree->GetBoundingBox();

            if (!tree->GetIsDestroyed() && SDL_HasIntersection(&playerBoundingBox, &treeBoundingBox)) {
                tree->Extinguish();

                scoreCounter->AddScore(100);
            }
        }
    }
}

void Player::Update() {
    if (visible) {
        UpdateSourceRectangle();
        UpdateDestinationRectangle();
    }
}

Player::Player(const char* imagePathRight, const char* imagePathLeft, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible, ScoreCounter* scoreCounter)
    : surfaceLeft(nullptr), surfaceRight(nullptr) {
    this->scoreCounter = scoreCounter;

    this->width = width;
    this->height = height;

    surfaceLeft = IMG_Load(imagePathRight);
    surfaceRight = IMG_Load(imagePathLeft);

    texture = SDL_CreateTextureFromSurface(renderer, surfaceRight);

    this->renderer = renderer;
    this->x = x;
    this->y = y;

    Reset(movementSpeed, visible);
}
