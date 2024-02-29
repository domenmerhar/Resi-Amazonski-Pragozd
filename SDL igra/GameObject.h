#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include "Util.h" 

class GameObject {
protected:
    int x, y, movementSpeed, width, height;
    bool visible;

    SDL_Texture* texture;
    SDL_Rect sourceRectangle, destinationRectangle;
    SDL_Renderer* renderer;

    const char* imagePathRight;

    void UpdateSourceRectangle();
    void UpdateDestinationRectangle();
    void Init(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible);

public:
    GameObject();
    GameObject(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible);
    GameObject(const char* imagePathRight, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible);

    void Update();
    void Render();
    void Reset(int movementSpeed, bool isVisible);
    void Hide();
    void Show();

    bool GetIsVisible();
    int GetX();
    int GetY();
    void SetPosition(int x, int y);
    int getHeight();
    int getWidth();
    SDL_Rect GetBoundingBox();
};