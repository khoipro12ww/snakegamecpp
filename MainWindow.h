#include "funcition.h"

#ifndef _MAINWINDOW_H
#define _MAINWINDOW_H

class Window {
public:
	SDL_Window* p_window = nullptr;
	SDL_Renderer* p_renderer = nullptr;
public:
	Window(std::string title, int w, int h);
	~Window();
	void Render();
	void free();
	void SetIcon(std::string &path);
	void Fill(Color& color);
};

Window::~Window() {
	free();
}

Window::Window(std::string title, int w, int h) {
	this->p_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_SHOWN);
	if (p_window == nullptr) {
		std::cout << "Can't initialization window \n";
	}
	this->p_renderer = SDL_CreateRenderer(p_window, -1, 0);
	if (p_renderer == nullptr) {
		std::cout << "Can't initialization renderer \n";
	}
}
void Window::SetIcon(std::string &path) {
	SDL_Surface* p_IconSurface = IMG_Load(path.c_str());
	SDL_SetWindowIcon(p_window, p_IconSurface);
	SDL_FreeSurface(p_IconSurface);
}

void Window::Fill(Color& color) {
	SDL_SetRenderDrawColor(p_renderer, color.r, color.g, color.b, 255);
	SDL_RenderClear(p_renderer);
}
void Window::free() {
	SDL_DestroyWindow(p_window);
	SDL_DestroyRenderer(p_renderer);
}

void Window::Render() {
	SDL_RenderPresent(this->p_renderer);
}

#endif // !_MAINWINDOW_H




