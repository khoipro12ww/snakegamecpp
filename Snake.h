#pragma once
#ifndef SNAKE_H
#define SNAKE_H
#include "funcition.h"
#include "food.h"

class Snake {
public:
	int x, y; bool die = false;
	SDL_Rect snake_rect;

public:
	Snake(int x, int y, int w, int h);
	void Draw(SDL_Renderer* &p_renderer, Color& color);
	void Update(int &x, int &y);
	bool Collidrect(Food& food);
};

Snake::Snake(int x, int y, int w, int h) {
	this->x = x; this->y = y;
	this->snake_rect.x = this->x;
	this->snake_rect.y = this->y;
	this->snake_rect.w = w;
	this->snake_rect.h = h;
}

void Snake::Draw(SDL_Renderer* &p_renderer,Color& color) {
	SDL_SetRenderDrawColor(p_renderer, color.r, color.g, color.b, 255);
	SDL_RenderFillRect(p_renderer, &snake_rect);
}

void Snake::Update(int &x, int &y) {
	this->x = x; this->y = y;
	this->snake_rect.x = this->x;
	this->snake_rect.y = this->y;
	this->snake_rect.w = 10;
	this->snake_rect.h = 10;
}

bool Snake::Collidrect(Food& food) {
	if (snake_rect.x == food.food_rect.x && snake_rect.y == food.food_rect.y) {
		return true;
	}
	else {
		return false;
	}
}

#endif // !SNAKE_H
