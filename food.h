#pragma once
#ifndef FOOD_H
#define FOOD_H
#include "funcition.h"

class Food {
public:
	int x, y, w, h;
	SDL_Rect food_rect;
public:
	Food(int w, int h);
	void Draw(SDL_Renderer* &p_renderer, Color& color);
	void Update();
};

Food::Food(int w, int h) {
	this->x = randrange(10); this->y = randrange(10);
	this->w = w;
	this->h = h;
	food_rect.x = this->x;
	food_rect.y = this->y;
	food_rect.w = this->w;
	food_rect.h = this->h;
}

void Food::Draw(SDL_Renderer* &p_renderer, Color& color) {
	SDL_SetRenderDrawColor(p_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(p_renderer, &food_rect);
}

void Food::Update() {
	this->x = randrange(10); this->y = randrange(10);
	food_rect.x = this->x;
	food_rect.y = this->y;
}

#endif // !FOOD_H
