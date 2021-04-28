#include "main.h"

#include <filesystem>
#include <iostream>
#include <optional>
#include <string>
#include <vector>

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define APP_NAME "Slideshow App"

std::vector<std::string> get_filenames_from_path(const std::string &path) {
  std::vector<std::string> out;

  if (!std::filesystem::is_directory(path)) return out;

  for (const auto &entry : std::filesystem::directory_iterator(path))
    out.emplace_back(entry.path());

  return out;
}

SDL_Rect fit_image(SDL_Window *window, SDL_Surface *surface) {
  SDL_Rect out;
  int window_w, window_h;
  SDL_GetWindowSize(window, &window_w, &window_h);

  out.x = 0;
  out.y = 0;
  out.w = surface->w;
  out.h = surface->h;

  if (window_h > surface->h && window_w > surface->w) {
    out.w = surface->w;
    out.h = surface->h;
    out.x = (window_w - surface->w) / 2;
    out.y = (window_h - out.h) / 2;
  } else if (window_h > surface->h && window_w < surface->w) {
    out.w = window_w;
    out.h = ((double)window_w / (double)surface->w) * surface->h;
    out.x = 0;
    out.y = (window_h - out.h) / 2;
  } else if (window_h < surface->h && window_w > surface->w) {
    out.w = ((double)window_h / (double)surface->h) * surface->w;
    out.h = window_h;
    out.x = (window_w - out.w) / 2;
    out.y = 0;
  }else if (window_h < surface->h && window_w > surface->w) {
    out.w = ((double)window_h / (double)surface->h) * surface->w;
    out.h = window_h;
    out.x = (window_w - out.w) / 2;
    out.y = 0;
  }

  return out;
}

int main(int argc, char **argv) {
  //obtain file names from assets
  auto filenames = get_filenames_from_path("assets");
  if (filenames.size() <= 0) {
    std::cerr << "Failed to load filenames!\n";
    return 1;
  }

  SDL_Init(SDL_INIT_EVERYTHING);
  IMG_Init(IMG_INIT_PNG);

  SDL_Window *window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

  SDL_Surface *current_image = IMG_Load(filenames[7].c_str());
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, current_image);

  int duration = 5000;

  int i = 0;

  SDL_Event event;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          break;
      }
    }

    if (quit) break;

    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);

    SDL_Rect r = fit_image(window, current_image);
    // SDL_Rect r = {0, 0, 720, 360};

    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    SDL_RenderClear(renderer);
    SDL_RenderCopy(renderer, texture, NULL, &r);
    SDL_RenderPresent(renderer);
  }

  SDL_DestroyTexture(texture);
  SDL_FreeSurface(current_image);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  return 0;
}
