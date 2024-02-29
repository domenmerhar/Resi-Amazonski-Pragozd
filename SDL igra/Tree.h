#pragma once
#include <SDL.h>
#include "Color.h"

class Tree {
private:
    int x, y, width, height;
    int spreadX, spreadY;
    int framesToDestruction;
    float timeToBurn;
    bool canSpread;
    float timeToExcavate;
    bool inDestruction;
    bool isDestroyed;
    bool canBeDestroyed;
    SDL_Texture* texture;
    SDL_Rect sourceRectangle, destinationRectangle;
    SDL_Renderer* renderer;
    SDL_Surface* tempSurface;
    Color treeColor, fireColor, excavationColor;

    void UpdateSourceRectangle();
    void UpdateDestinationRectangle();
    void Burn();
    void SetColor(int red, int green, int blue);
    void ResetFramesToBurn();
    void ResetFramesToExcavate();

public:
    Tree();
    Tree(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int width, int height, float timeToBurn, int FPS, Color treeColor, Color fireColor, Color excavationColor);
    void Update();
    void Render();
    void Init(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int width, int height, float timeToBurn, int FPS, Color treeColor, Color fireColor, Color excavationColor);
    void Reset(float timeToBurn, int red, int green, int blue);
    int GetX();
    int GetY();
    int GetSpreadX();
    int GetSpreadY();
    void SetPosition(int x, int y);
    int getHeight();
    int getWidth();
    SDL_Rect GetBoundingBox();
    void Extinguish();
    bool GetIsDestroyed();
    bool GetIsInDestruction();
    void StartBurning();
    void StartExcavating();
};
