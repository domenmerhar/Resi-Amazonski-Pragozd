#include "Ally.h"

void Ally::GenerateRandomTarget() {
    targetX = Util::GetRandomX(width);
    targetY = Util::GetRandomY(height);
}

void Ally::Init(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height) {
    this->width = width;
    this->height = height;

    SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
    SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    this->renderer = renderer;

    Reset(movementSpeed);
}

Ally::Ally(int red, int green, int blue, SDL_Renderer* renderer, int movementSpeed, int width, int height) {
    Init(red, green, blue, renderer, movementSpeed, width, height);
}

Ally::Ally(const char* imagePathRight, const char* imagePathLeft, SDL_Renderer* renderer, int movementSpeed, int width, int height) {
    this->width = width;
    this->height = height;

    surfaceRight = IMG_Load(imagePathRight);
    surfaceLeft = IMG_Load(imagePathLeft);

    texture = SDL_CreateTextureFromSurface(renderer, surfaceRight);

    this->renderer = renderer;

    Reset(movementSpeed);
}

void Ally::Reset(int movementSpeed) {
    spawnX = x = Util::GetRandomX(width);
    spawnY = y = Util::GetRandomY(height);
    this->movementSpeed = movementSpeed;

    UpdateSourceRectangle();
    visible = true;

    GenerateRandomTarget();
}

void Ally::Move() {
    float dx = targetX - x;
    float dy = targetY - y;

    float magnitude = sqrt(dx * dx + dy * dy);

    if (magnitude > 0) {
        dx /= magnitude;
        dy /= magnitude;

        x += round(dx * movementSpeed);
        y += round(dy * movementSpeed);

        if (dx > 0) {
            texture = SDL_CreateTextureFromSurface(renderer, surfaceLeft);
        }
        else if (dx < 0) {
            texture = SDL_CreateTextureFromSurface(renderer, surfaceRight);
        }
    }
    else {
        GenerateRandomTarget();
    }
}

void Ally::Hide() {
    visible = false;
    x = spawnX;
    y = spawnY;
}

bool Ally::IsInRange(int x, int y, int range) {
    int distanceSquared = (this->x - x) * (this->x - x) + (this->y - y) * (this->y - y);
    int rangeSquared = range * range;

    return distanceSquared <= rangeSquared;
}

bool Ally::IsTreeInRange(Tree* tree, int range) {
    if (!tree->GetIsInDestruction()) return false;

    int treeX = tree->GetX() + tree->getWidth() / 2;
    int treeY = tree->GetY() + tree->getHeight() / 2;

    return IsInRange(treeX, treeY, range);
}

void Ally::SetTarget(int x, int y, int width, int height) {
    targetX = x + width / 2;
    targetY = y + height / 2;
}

void Ally::HandleTreeCollision(std::vector<Tree*> treesInDestruction) {
    for (Tree* tree : treesInDestruction) {
        SDL_Rect allyBoundingBox = GetBoundingBox();
    }

    for (Tree* tree : treesInDestruction) {
        if (visible && tree != nullptr) {
            SDL_Rect allyBoundingBox = GetBoundingBox();
            SDL_Rect treeBoundingBox = tree->GetBoundingBox();

            if (tree->GetIsInDestruction() && SDL_HasIntersection(&allyBoundingBox, &treeBoundingBox)) {
                tree->Extinguish();
                GenerateRandomTarget();
                break;
            }
        }
    }
}
