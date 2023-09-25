#pragma once
#ifndef TEXT_H
#define TEXT_H
#include"funcition.h"

class Text {
public:
	SDL_Texture* p_TextureFont = nullptr;
	SDL_Surface* SurfaceMsg = nullptr;
	TTF_Font* Sans = nullptr;
	SDL_Rect RenderQad{};
	std::string txt;
public:
	Text(std::string &path, SDL_Renderer* &p_renderer, std::string render, SDL_Color& color, int x, int y,int w,int h);
	void Render(SDL_Renderer* &p_renderer);
	void Update(SDL_Renderer* &p_renderer, std::string new_str, std::string& path, SDL_Color& color);
};

Text::Text(std::string &path,SDL_Renderer* &p_renderer , std::string render, SDL_Color& color,int x,int y, int w, int h) {
	TTF_Init();
	this->txt = render;
	this->Sans = TTF_OpenFont(path.c_str(), 15);
	this->SurfaceMsg = TTF_RenderText_Solid(Sans, this->txt.c_str(), color);
	this->p_TextureFont = SDL_CreateTextureFromSurface(p_renderer, SurfaceMsg);
	this->RenderQad = {x, y, w, h};
	SDL_FreeSurface(this->SurfaceMsg);

}

void Text::Render(SDL_Renderer* &p_renderer) {
	SDL_RenderCopy(p_renderer, p_TextureFont, NULL, &RenderQad);
}

void Text::Update(SDL_Renderer* &p_renderer, std::string new_str,std::string &path, SDL_Color& color) {
	this->txt = new_str;
	this->SurfaceMsg = TTF_RenderText_Solid(Sans, this->txt.c_str(), color);
	this->p_TextureFont = SDL_CreateTextureFromSurface(p_renderer, SurfaceMsg);
	SDL_FreeSurface(this->SurfaceMsg);
}

#endif // !TEXT_H
