#pragma once
#ifndef _FUNCITION_H_
#define _FUNCITION_H_
#include <random>
#include <SDL.h>
#include <SDL_image.h>
#include <deque>
#include <string>
#include <SDL_ttf.h>
#include <iostream>

struct Color {
	Uint8 r, g, b;
	Color(int r, int g, int b) : r(r), b(b), g(g) {}
};


Color RED = Color(255, 0, 0);
Color BLACK = Color(0, 0, 0);
Color WHITE = Color(255, 255, 255);
Color BlueF = Color(0, 0, 255);
SDL_Color _WHITE = { 255,255,255 };
SDL_Event g_event;


static const int MAX_WIDTH = 800;
static const int MAX_HEIGHT = 800;
static const int FPS = 15;

bool running = true;

int block_x = 50, block_y = 50;

int x_vel = 0; int y_vel = 0;

int lenght_snake = 1;

int score = 0;

static std::random_device rd;
static std::uniform_int_distribution<int> dist(0, MAX_WIDTH - 10);
static std::string path_image = "images.png";
static std::string path_font = "OpenSans-Bold.ttf";

static int randrange(int step) {
	int temp = dist(rd);
	int result = (10 - (temp % 10)) + temp;
	return result;
}

#endif