#include "Player.h"

void Player::HandleInput() {
    if (visible) {
        const float diagonalSpeedMultiplier = 0.7071;

        SDL_PumpEvents();

        const Uint8* keys = SDL_GetKeyboardState(NULL);

        float dx = 0, dy = 0;

        if (keys[SDL_SCANCODE_UP]) {
            dy -= 1;
        }
        if (keys[SDL_SCANCODE_DOWN]) {
            dy += 1;
        }
        if (keys[SDL_SCANCODE_LEFT]) {
            dx -= 1;
        }
        if (keys[SDL_SCANCODE_RIGHT]) {
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

Player::Player(const char* imagePathRight, const char* imagePathLeft, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible)
    : surfaceLeft(nullptr), surfaceRight(nullptr) {
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
