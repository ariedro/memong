#include <SDL2/SDL.h>
#include <stdio.h>

const int SCREEN_WIDTH = 640;
const int SCREEN_HEIGHT = 480;

bool init();
bool loadMedia();
void close();

SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;

bool init() {
  bool success = true;

  if (SDL_Init(SDL_INIT_VIDEO) < 0) {
    printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  if (!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "1"))
    printf("Warning: Linear texture filtering not enabled!");

  gWindow = SDL_CreateWindow("Memong", SDL_WINDOWPOS_UNDEFINED,
                             SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
                             SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
  if (gWindow == NULL) {
    printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  gRenderer = SDL_CreateRenderer(gWindow, -1, SDL_RENDERER_PRESENTVSYNC);
  if (gRenderer == NULL) {
    printf("Renderer could not be created! SDL Error: %s\n", SDL_GetError());
    return false;
  }

  SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);

  return true;
}

void close() {
  SDL_DestroyRenderer(gRenderer);
  SDL_DestroyWindow(gWindow);
  gWindow = NULL;
  gRenderer = NULL;
  SDL_Quit();
}

int main(int argc, char *args[]) {
  if (!init()) {
    printf("Failed to initialize!\n");
    close();
    return 1;
  }
  bool quit = false;
  SDL_Event e;

  // Init

  const uint BALL_SIZE = 16;
  const uint STICK_HEIGHT = 64;
  const uint STICK_WIDTH = 16;
  const uint STICK_OFFSET = 8;
  const uint MOVE_DELTA = 32;

  float ball_pos[] = {SCREEN_WIDTH / 2, SCREEN_HEIGHT / 2};
  float ball_vel[] = {2, -2};
  float player_1_pos = 0;
  float player_2_pos = 0;
  bool lost = false;

  // Controller

  while (!quit) {
    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT)
        quit = true;
      else if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
        case SDLK_w:
          if (player_1_pos > 0)
            player_1_pos -= MOVE_DELTA;
          break;
        case SDLK_s:
          if (player_1_pos + 2 * MOVE_DELTA < SCREEN_HEIGHT)
            player_1_pos += MOVE_DELTA;
          break;
        case SDLK_UP:
          if (player_2_pos > 0)
            player_2_pos -= MOVE_DELTA;
          break;
        case SDLK_DOWN:
          if (player_2_pos + 2 * MOVE_DELTA < SCREEN_HEIGHT)
            player_2_pos += MOVE_DELTA;
          break;
        }
      }
    }

    // Game logic in aids style

    if (((ball_pos[0] < BALL_SIZE + STICK_OFFSET) &&
         (ball_pos[1] < player_1_pos + STICK_HEIGHT) &&
         (ball_pos[1] > player_1_pos)) ||
        ((ball_pos[0] >
          SCREEN_WIDTH - BALL_SIZE - STICK_OFFSET - STICK_WIDTH) &&
         (ball_pos[1] < player_2_pos + STICK_HEIGHT) &&
         (ball_pos[1] > player_2_pos))) {
      ball_vel[0] *= -1.2;
      ball_vel[1] *= 1.2;
    }

    if ((ball_pos[1] + ball_vel[1] < 0) ||
        (ball_pos[1] + ball_vel[1] > SCREEN_HEIGHT - BALL_SIZE))
      ball_vel[1] *= -1;

    if (ball_pos[0] < 0) {
      ball_pos[0] = SCREEN_WIDTH / 2;
      ball_pos[1] = SCREEN_HEIGHT / 2;
      ball_vel[0] = 2;
      ball_vel[1] = 2;
    }

    if ((ball_pos[0] + BALL_SIZE) > SCREEN_WIDTH) {
      ball_pos[0] = SCREEN_WIDTH / 2;
      ball_pos[1] = SCREEN_HEIGHT / 2;
      ball_vel[0] = -2;
      ball_vel[1] = 2;
    }

    ball_pos[0] += ball_vel[0];
    ball_pos[1] += ball_vel[1];

    // Render

    SDL_SetRenderDrawColor(gRenderer, 0x00, 0x00, 0x00, 0xFF);
    SDL_RenderClear(gRenderer);

    SDL_SetRenderDrawColor(gRenderer, 0x80, 0x80, 0x80, 0xFF);
    for (uint i = 8; i < SCREEN_HEIGHT - 8; i += 20) {
      SDL_RenderDrawLine(gRenderer, SCREEN_WIDTH / 2, i, SCREEN_WIDTH / 2,
                         i + 8);
    }

    SDL_Rect fillRect;

    fillRect = {ball_pos[0], ball_pos[1], BALL_SIZE, BALL_SIZE};
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(gRenderer, &fillRect);

    fillRect = {STICK_OFFSET, player_1_pos, STICK_WIDTH, STICK_HEIGHT};
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(gRenderer, &fillRect);

    fillRect = {SCREEN_WIDTH - STICK_WIDTH - STICK_OFFSET, player_2_pos,
                STICK_WIDTH, STICK_HEIGHT};
    SDL_SetRenderDrawColor(gRenderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_RenderFillRect(gRenderer, &fillRect);

    SDL_RenderPresent(gRenderer);
  }

  close();

  return 0;
}