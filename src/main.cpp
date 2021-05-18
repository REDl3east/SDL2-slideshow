#include "main.h"

#define SCREEN_WIDTH 1280
#define SCREEN_HEIGHT 720
#define APP_NAME "Slideshow App"

#define BTN_TIMER 1000
#define BTN_TIMERCODE 0

std::vector<std::string> get_filenames_from_path(const std::string &path);
void fit_image(SDL_Window *window, SDL_Surface *surface, SDL_Rect &rect_out);
void fit_image(SDL_Rect &window_rect, SDL_Rect &surface_rect, SDL_Rect &out_rect);

Uint32 btn_timer_callback(Uint32 interval, void *param) {
  SDL_Event event;
  SDL_UserEvent userevent;

  userevent.type = SDL_USEREVENT;
  userevent.code = BTN_TIMERCODE;

  event.type = SDL_USEREVENT;
  event.user = userevent;

  SDL_PushEvent(&event);
  return 0;
}

int main(int argc, char **argv) {
  //obtain file names from assets
  auto filenames = get_filenames_from_path("assets");
  if (filenames.size() <= 0) {
    std::cerr << "Failed to load filenames!\n";
    return 1;
  }

  SDL_Init(SDL_INIT_EVERYTHING);
  IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG);

  SDL_Window *window = SDL_CreateWindow(APP_NAME, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_RESIZABLE);
  SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, 0);
  SDL_Surface *current_image = IMG_Load(filenames[0].c_str());
  SDL_Texture *current_texture = SDL_CreateTextureFromSurface(renderer, current_image);

  SDL_SetWindowTitle(window, filenames[0].c_str());
  SDL_SetWindowMinimumSize(window, 250, 250);

  int index = 0;
  auto increment_index = [&index, &filenames]() {
    index = (index + 1) % filenames.size();
  };
  auto decrement_index = [&index, &filenames]() {
    index = (index - 1) % filenames.size();
  };
  auto update_texture = [&]() {
    SDL_DestroyTexture(current_texture);
    SDL_FreeSurface(current_image);
    current_image = IMG_Load(filenames[index].c_str());
    current_texture = SDL_CreateTextureFromSurface(renderer, current_image);
    SDL_SetWindowTitle(window, filenames[index].c_str());
  };

  auto btn_surface = IMG_Load("assets/button/btn.png");
  auto btn_texture = SDL_CreateTextureFromSurface(renderer, btn_surface);
  TwoDirectionalButton right_btn(renderer, btn_texture, 0, 0, 100, 100, 0.5);
  TwoDirectionalButton left_btn(renderer, btn_texture, 0, 0, 100, 100, 0.5, Direction::Flipped);

  SDL_TimerID btn_timer_id;

  SDL_Event event;
  bool quit = false;
  while (!quit) {
    while (SDL_PollEvent(&event)) {
      switch (event.type) {
        case SDL_QUIT:
          quit = true;
          break;
        case SDL_KEYDOWN:
          if (event.key.keysym.sym == SDLK_LEFT) {
            decrement_index();
            update_texture();
          } else if (event.key.keysym.sym == SDLK_RIGHT) {
            increment_index();
            update_texture();
          }
          break;
        case SDL_MOUSEMOTION: {
          left_btn.set_visible(true);
          right_btn.set_visible(true);
          bool off_btn = true;
          SDL_Point p = {event.motion.x, event.motion.y};
          if (SDL_PointInRect(&p, &left_btn.get_rect()) == SDL_TRUE) {
            SDL_RemoveTimer(btn_timer_id);
            left_btn.update_state(State::Hovered);
            off_btn = false;
          } else {
            left_btn.update_state(State::Default);
          }
          if (SDL_PointInRect(&p, &right_btn.get_rect()) == SDL_TRUE) {
            SDL_RemoveTimer(btn_timer_id);
            right_btn.update_state(State::Hovered);
            off_btn = false;
          } else {
            right_btn.update_state(State::Default);
          }

          if (off_btn) {
            SDL_RemoveTimer(btn_timer_id);
            btn_timer_id = SDL_AddTimer(BTN_TIMER, btn_timer_callback, NULL);
          }

          break;
        }
        case SDL_MOUSEBUTTONUP: {
          SDL_Point p = {event.button.x, event.button.y};
          if (SDL_PointInRect(&p, &left_btn.get_rect()) == SDL_TRUE) {
            decrement_index();
            update_texture();
          } else if (SDL_PointInRect(&p, &right_btn.get_rect()) == SDL_TRUE) {
            increment_index();
            update_texture();
          }
          break;
        }
        case SDL_USEREVENT: {
          if (event.user.code == BTN_TIMERCODE) {
            left_btn.set_visible(false);
            right_btn.set_visible(false);
          }
          break;
        }
      }
    }

    if (quit) break;

    int window_w, window_h;
    SDL_GetWindowSize(window, &window_w, &window_h);

    left_btn.update_pos((left_btn.width() / 2) + 10, window_h / 2);
    right_btn.update_pos(window_w - (right_btn.width() / 2) - 10, window_h / 2);

    SDL_Rect r;
    fit_image(window, current_image, r);

    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
    SDL_RenderClear(renderer);

    SDL_RenderCopy(renderer, current_texture, NULL, &r);

    left_btn.render();
    right_btn.render();

    SDL_RenderPresent(renderer);
  }

  SDL_FreeSurface(btn_surface);
  SDL_DestroyTexture(btn_texture);

  SDL_DestroyTexture(current_texture);
  SDL_FreeSurface(current_image);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  IMG_Quit();
  SDL_Quit();

  return 0;
}

std::vector<std::string> get_filenames_from_path(const std::string &path) {
  std::vector<std::string> out;

  if (!std::filesystem::is_directory(path)) return out;

  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    if (!entry.is_directory()) {
      out.emplace_back(entry.path());
    }
  }

  return out;
}

void fit_image(SDL_Window *window, SDL_Surface *surface, SDL_Rect &rect_out) {
  int window_w, window_h;
  SDL_GetWindowSize(window, &window_w, &window_h);

  SDL_Rect window_rect = {0, 0, window_w, window_h};
  SDL_Rect surface_rect = surface->clip_rect;

  fit_image(window_rect, surface_rect, rect_out);
}

void fit_image(SDL_Rect &window_rect, SDL_Rect &surface_rect, SDL_Rect &out_rect) {
  if (window_rect.h >= surface_rect.h && window_rect.w >= surface_rect.w) {
    out_rect.w = surface_rect.w;
    out_rect.h = surface_rect.h;
    out_rect.x = (window_rect.w - surface_rect.w) / 2;
    out_rect.y = (window_rect.h - out_rect.h) / 2;
  } else if (window_rect.h >= surface_rect.h && window_rect.w <= surface_rect.w) {
    out_rect.w = window_rect.w;
    out_rect.h = ((double)window_rect.w / (double)surface_rect.w) * surface_rect.h;
    out_rect.x = 0;
    out_rect.y = (window_rect.h - out_rect.h) / 2;
  } else if (window_rect.h <= surface_rect.h && window_rect.w >= surface_rect.w) {
    out_rect.w = ((double)window_rect.h / (double)surface_rect.h) * surface_rect.w;
    out_rect.h = window_rect.h;
    out_rect.x = (window_rect.w - out_rect.w) / 2;
    out_rect.y = 0;
  } else if (window_rect.h <= surface_rect.h && window_rect.w <= surface_rect.w) {
    //calculate the new surface
    surface_rect.w = ((double)window_rect.h / (double)surface_rect.h) * surface_rect.w;
    surface_rect.h = window_rect.h;
    fit_image(window_rect, surface_rect, out_rect);
  }
}