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

int main(int argc, char **argv) {
  //obtain file names from assets
  auto filenames = get_filenames_from_path("assets");
  if (filenames.size() <= 0) {
    std::cerr << "Failed to load filenames!\n";
    return 1;
  }

  SDL_Init(SDL_INIT_EVERYTHING);
  IMG_Init(IMG_INIT_PNG);

  SDL_Window *window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE & 0);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);

  auto *current_image = IMG_Load(filenames[0].c_str());
  SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, current_image);

  SDL_Event event;
  bool quit = false;
  while (!quit) {
    SDL_WaitEvent(&event);

    switch (event.type) {
      case SDL_QUIT:
        quit = true;
        break;
    }

    SDL_RenderCopy(renderer, texture, NULL, NULL);
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
