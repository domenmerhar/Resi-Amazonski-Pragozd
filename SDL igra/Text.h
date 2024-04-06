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
	Text(const char*, int, int, SDL_Renderer*, const char*, bool, SDL_Color);

	void Render();
	void ChangeText(const char*);
};

