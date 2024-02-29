#include "GameObject.h"

void GameObject::UpdateSourceRectangle() {
    sourceRectangle.h = height;
    sourceRectangle.w = width;
    sourceRectangle.x = 0;
    sourceRectangle.y = 0;
}

void GameObject::UpdateDestinationRectangle() {
    if (Util::WithinBoundsX(x, width))
        destinationRectangle.x = x;
    else
        x = destinationRectangle.x = Util::ResetX(x, width);

    if (Util::WithinBoundsY(y, height * 2))
        destinationRectangle.y = y;
    else
        y = destinationRectangle.y = Util::ResetY(y, height * 2);

    destinationRectangle.w = sourceRectangle.w * 2;
    destinationRectangle.h = sourceRectangle.h * 2;
}

void GameObject::Init(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible) {
    this->width = width;
    this->height = height;

    SDL_Surface* tempSurface = SDL_CreateRGBSurface(0, width, height, 16, 0, 0, 0, 0);
    SDL_FillRect(tempSurface, NULL, SDL_MapRGB(tempSurface->format, red, green, blue));
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    this->renderer = renderer;
    this->x = x;
    this->y = y;

    Reset(movementSpeed, visible);
}

GameObject::GameObject() {
    Init(255, 255, 255, NULL, 0, 0, 0, 32, 32, true);
}

GameObject::GameObject(int red, int green, int blue, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible) {
    Init(red, green, blue, renderer, x, y, movementSpeed, width, height, visible);
}

GameObject::GameObject(const char* imagePathRight, SDL_Renderer* renderer, int x, int y, int movementSpeed, int width, int height, bool visible) {
    this->width = width;
    this->height = height;

    this->imagePathRight = imagePathRight;

    SDL_Surface* tempSurface = IMG_Load(imagePathRight);
    texture = SDL_CreateTextureFromSurface(renderer, tempSurface);
    SDL_FreeSurface(tempSurface);

    this->renderer = renderer;
    this->x = x;
    this->y = y;

    Reset(movementSpeed, visible);
}

void GameObject::Update() {
    if (visible) {
        UpdateSourceRectangle();
        UpdateDestinationRectangle();
    }
}

void GameObject::Render() {
    if (visible) SDL_RenderCopy(renderer, texture, &sourceRectangle, &destinationRectangle);
}

void GameObject::Reset(int movementSpeed, bool isVisible) {
    this->movementSpeed = movementSpeed;
    visible = isVisible;

    UpdateSourceRectangle();
}

void GameObject::Hide() {
    visible = false;
}

void GameObject::Show() {
    visible = true;
}

bool GameObject::GetIsVisible() {
    return visible;
}

int GameObject::GetX() {
    return x;
}

int GameObject::GetY() {
    return y;
}

void GameObject::SetPosition(int x, int y) {
    this->x = x;
    this->y = y;
}

int GameObject::getHeight() {
    return height;
}

int GameObject::getWidth() {
    return width;
}

SDL_Rect GameObject::GetBoundingBox() {
    SDL_Rect boundingBox = { x, y, width, height };
    return boundingBox;
}
