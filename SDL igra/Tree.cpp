#include "Tree.h"
#include "Util.h"
/*
Tree::Tree() {
    Init(255, 255, 255, nullptr, 0, 0, 32, 32, 5, Util::FPS, forestGreen, orange, gray);
}
*/
Tree::Tree(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int width, int height, float timeToBurn, int FPS,
    Color treeColor, Color fireColor, Color excavationColor) {
    Init(red, green, blue, renderer, x, y, width, height, timeToBurn, FPS, treeColor, fireColor, excavationColor);
}

void Tree::UpdateSourceRectangle() {
    sourceRectangle.h = height;
    sourceRectangle.w = width;
    sourceRectangle.x = 0;
    sourceRectangle.y = 0;
}

void Tree::UpdateDestinationRectangle() {
    destinationRectangle.x = x;
    destinationRectangle.y = y;

    destinationRectangle.w = sourceRectangle.w * 2;
    destinationRectangle.h = sourceRectangle.h * 2;
}

void Tree::Burn() {
    framesToDestruction--;

    if (framesToDestruction <= 100 && canSpread) {
        canSpread = false;
        if (true) {
            int combination = rand() % 9;

            switch (combination) {
            case 0:
                spreadX = x + 64;
                spreadY = y + 64;
                break;
            case 1:
                spreadX = x + 64;
                spreadY = y + 0;
                break;
            case 2:
                spreadX = x + 64;
                spreadY = y - 64;
                break;
            case 3:
                spreadX = x + 0;
                spreadY = y + 64;
                break;
            case 4:
                spreadX = -1;
                spreadY = -1;
                break;
            case 5:
                spreadX = x + 0;
                spreadY = y - 64;
                break;
            case 6:
                spreadX = x - 64;
                spreadY = y + 64;
                break;
            case 7:
                spreadX = x - 64;
                spreadY = y + 0;
                break;
            case 8:
                spreadX = x - 64;
                spreadY = y - 64;
                break;
            }
        }
    }

    if (framesToDestruction <= 0) {
        inDestruction = false;

        tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
        SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, 88, 57, 39));
        texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
        SDL_FreeSurface(tempSurface);

        isDestroyed = true;
    }
}

void Tree::SetColor(int red, int green, int blue) {
    SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
    SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

void Tree::ResetFramesToBurn() {
    framesToDestruction = timeToBurn * Util::FPS;
}

void Tree::ResetFramesToExcavate() {
    framesToDestruction = timeToExcavate * Util::FPS;
}

void Tree::Update() {
    UpdateSourceRectangle();
    UpdateDestinationRectangle();

    if (canBeDestroyed && inDestruction) Burn();
}

void Tree::Render() {
    SDL_RenderCopy(renderer, texture, &sourceRectangle, &destinationRectangle);
}

void Tree::Init(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int width, int height, float timeToBurn, int FPS,
    Color treeColor, Color fireColor, Color excavationColor) {
    this->width = width;
    this->height = height;

    this->treeColor = treeColor;
    this->fireColor = fireColor;
    this->excavationColor = excavationColor;

    tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
    SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, treeColor.red, treeColor.green, treeColor.blue));
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    this->renderer = renderer;
    this->x = x;
    this->y = y;

    UpdateSourceRectangle();

    this->timeToBurn = timeToBurn;
    timeToExcavate = timeToBurn * 2;

    ResetFramesToBurn();

    isDestroyed = false;
    inDestruction = canBeDestroyed = false;

    spreadX = -1;
    spreadY = -1;
    canSpread = true;
}

void Tree::Reset(float timeToBurn, int red, int green, int blue) {
    tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
    SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, treeColor.red, treeColor.green, treeColor.blue));
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    this->timeToBurn = timeToBurn;
    timeToExcavate = timeToBurn * 2;

    isDestroyed = false;
    inDestruction = canBeDestroyed = false;
    spreadX = -1;
    spreadY = -1;
    canSpread = true;
}

int Tree::GetX() {
    return x;
}

int Tree::GetY() {
    return y;
}

int Tree::GetSpreadX() {
    return spreadX;
}

int Tree::GetSpreadY() {
    return spreadY;
}

void Tree::SetPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

int Tree::getHeight() {
    return height;
}

int Tree::getWidth() {
    return width;
}

SDL_Rect Tree::GetBoundingBox() {
    SDL_Rect boundingBox = { x - width / 2, y - height / 2, width, height };
    return boundingBox;
}

void Tree::Extinguish() {
    canBeDestroyed = inDestruction = false;

    tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
    SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, treeColor.red, treeColor.green, treeColor.blue));
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

bool Tree::GetIsDestroyed() {
    return isDestroyed;
}

bool Tree::GetIsInDestruction() {
    return inDestruction;
};

void Tree::StartBurning() {
    inDestruction = true;
    canBeDestroyed = true;
    ResetFramesToBurn();

    tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
    SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, fireColor.red, fireColor.green, fireColor.blue));
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

void Tree::StartExcavating() {
    inDestruction = true;
    canBeDestroyed = true;
    ResetFramesToExcavate();

    tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
    SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, excavationColor.red, excavationColor.green, excavationColor.blue));
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);
}

