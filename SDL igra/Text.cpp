#pragma once
#include <SDL.h>
#include <SDL_ttf.h>

#include "Text.h";

Text::Text(const char* text, int x, int y, SDL_Renderer* renderer, const char* fontPath, bool isVisible, SDL_Color textColor) {
	this->x = x;
	this->y = y;
	this->renderer = renderer;
	this->font = font;
	this->isVisible = isVisible;
	this->textColor = textColor;

	this->font = TTF_OpenFont(fontPath, 24);
	SDL_Surface* surface = TTF_RenderText_Solid(font, text, textColor);
	texture = SDL_CreateTextureFromSurface(renderer, surface);

	rect.x = x;
	rect.y = y;
	rect.w = surface->w;
	rect.h = surface->h;
}

void Text::Render() {
	if (!isVisible) return;

	SDL_RenderCopy(renderer, texture, NULL, &rect);
}

void Text::ChangeText(const char* text) {
	SDL_DestroyTexture(texture);

	SDL_Surface* surface = TTF_RenderText_Solid(font, text, textColor);
	texture = SDL_CreateTextureFromSurface(renderer, surface);

	rect.w = surface->w;
	rect.h = surface->h;
}