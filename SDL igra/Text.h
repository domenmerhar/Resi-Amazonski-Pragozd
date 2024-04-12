#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

class Text {
	int x;
	int y;
	bool isVisible;

	SDL_Renderer* renderer;
	TTF_Font* font;
	SDL_Texture* texture;
	SDL_Rect rect;
	SDL_Color textColor;

public:
	Text(const char* text, int x, int y, SDL_Renderer* renderer, const char* fontPath, bool isVisible, SDL_Color textColor);

	void Render();
	void ChangeText(const char*);
	SDL_Rect GetRect();
	bool IsColliding(int x, int y);
};

