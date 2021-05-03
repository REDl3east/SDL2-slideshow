#ifndef BUTTON_H
#define BUTTON_H

#include <string>

#include <SDL.h>
#include <SDL_image.h>

class Button {
 public:
  Button(SDL_Renderer *r, const std::string &path, int x, int y, double scale = 1.0) {
    this->x = x;
    this->y = y;
    this->scale = scale;
    renderer = r;
    button_surface = IMG_Load(path.c_str());
    button_texture = SDL_CreateTextureFromSurface(renderer, button_surface);
  }
  ~Button() {
    SDL_DestroyTexture(button_texture);
    SDL_FreeSurface(button_surface);
  }
  void render() {
    SDL_Rect r = {(int)(x - scale * (button_surface->w / 2)), (int)(y - scale * (button_surface->h / 2)), (int)(button_surface->w * scale), (int)(button_surface->h * scale)};
    SDL_RenderCopy(renderer, button_texture, NULL, &r);
  }
  void update_pos(int x, int y) {
    this->x = x;
    this->y = y;
  }

  int width() {
    return button_surface->w * scale;
  }
  int height() {
    return button_surface->h * scale;
  }

 private:
  int x, y;
  double scale;
  SDL_Surface *button_surface;
  SDL_Texture *button_texture;
  SDL_Renderer *renderer;
};

#endif