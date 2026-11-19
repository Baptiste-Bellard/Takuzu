#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"
#include "model.h"

/* **************************************************************** */

int main(int argc, char* argv[]) {
  if (SDL_Init(SDL_INIT_VIDEO) != 0)
    ERROR("Error: SDL_Init VIDEO (%s)", SDL_GetError());
  if (IMG_Init(IMG_INIT_PNG | IMG_INIT_JPG) != (IMG_INIT_PNG | IMG_INIT_JPG))
    ERROR("Error: IMG_Init PNG (%s)", SDL_GetError());
  if (TTF_Init() != 0) ERROR("Error: TTF_Init (%s)", SDL_GetError());
  game g = game_default();
  if (argc == 2) {
    g = game_load(argv[1]);
  }
  SDL_Window* win = SDL_CreateWindow(
      APP_NAME, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH,
      SCREEN_HEIGHT, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
  if (!win) ERROR("Error: SDL_CreateWindow (%s)", SDL_GetError());
  SDL_Renderer* ren = SDL_CreateRenderer(
      win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
  if (!ren) ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_SOFTWARE);
  if (!ren) ERROR("Error: SDL_CreateRenderer (%s)", SDL_GetError());

  Env* env = init(win, ren, g);
  SDL_Event e;
  bool quit = false;

  while (!quit) {
    while (SDL_PollEvent(&e)) {
      quit = process(win, ren, env, &e);
      if (quit) break;
    }
    int w = 0, h = 0;
    SDL_GetWindowSize(win, &w, &h);
    SDL_RenderSetLogicalSize(ren, w, h);

    render(win, ren, env);
    SDL_RenderPresent(ren);
    SDL_Delay(DELAY);
  }
  if (game_is_over(g)) {
    printf("Congratulation!\n");
  }

  clean(win, ren, env);

  SDL_DestroyRenderer(ren);
  SDL_DestroyWindow(win);
  IMG_Quit();
  TTF_Quit();
  SDL_Quit();

  return EXIT_SUCCESS;
}
