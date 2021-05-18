#ifndef BUTTON_H
#define BUTTON_H

#include <SDL.h>
#include <SDL_image.h>

#include <string>

enum class Direction : int {
  Default = 0,
  Flipped = 1
};
enum class State : int {
  Default = 0,
  Hovered = 1
};

class TwoDirectionalButton {
 public:
  TwoDirectionalButton(SDL_Renderer *r, SDL_Texture *t, int x, int y, int w, int h, double scale = 1.0, Direction d = Direction::Default, State s = State::Default) {
    renderer = r;
    texture = t;
    this->x = x;
    this->y = y;
    this->w = w;
    this->h = h;
    this->scale = scale;
    direction = d;
    state = s;
  }
  ~TwoDirectionalButton() {
  }
  void render() {
    SDL_RenderCopy(renderer, texture, &button_rect, &window_rect);
  }
  void update_pos(int x, int y) {
    this->x = x;
    this->y = y;
    button_rect = {w * ((int)direction), h * ((int)state), w, h};
    window_rect = {(int)(x - scale * (w / 2)), (int)(y - scale * (h / 2)), (int)(w * scale), (int)(h * scale)};
  }
  void update_state(State s) {
    state = s;
  }
  void update_direction(Direction d) {
    direction = d;
  }

  int width() {
    return w * scale;
  }
  int height() {
    return h * scale;
  }

  SDL_Rect& get_rect(){
    return window_rect;
  }

 private:
  int x, y, w, h;
  double scale;
  SDL_Texture *texture;
  SDL_Renderer *renderer;

  Direction direction;
  State state;
  SDL_Rect button_rect;
  SDL_Rect window_rect;
};

// enum class State {
//   IDLE,
//   HOVERED
// };

// class SimpleButton {
//  public:
//   SimpleButton(SDL_Renderer *r, const std::string &idle_path, const std::string &hovered_path, int x, int y, double scale = 1.0) {
//     this->x = x;
//     this->y = y;
//     this->scale = scale;
//     renderer = r;
//     idle_button_surface = IMG_Load(idle_path.c_str());
//     idle_button_texture = SDL_CreateTextureFromSurface(renderer, idle_button_surface);
//     state = IDLE;
//   }
//   ~SimpleButton() {
//     SDL_DestroyTexture(idle_button_texture);
//     SDL_FreeSurface(idle_button_surface);
//   }
//   void render() {
//     SDL_Rect r = {(int)(x - scale * (idle_button_surface->w / 2)), (int)(y - scale * (idle_button_surface->h / 2)), (int)(idle_button_surface->w * scale), (int)(idle_button_surface->h * scale)};
//     SDL_RenderCopy(renderer, idle_button_texture, NULL, &r);
//   }
//   void update_pos(int x, int y) {
//     this->x = x;
//     this->y = y;
//   }

//   int width() {
//     return idle_button_surface->w * scale;
//   }
//   int height() {
//     return idle_button_surface->h * scale;
//   }

//   void setState(State s) {
//     state = s;
//   }

//  private:
//   int x, y;
//   double scale;
//   SDL_Surface *idle_button_surface;
//   SDL_Texture *idle_button_texture;
//   SDL_Renderer *renderer;

//   State state;
// };

#endif