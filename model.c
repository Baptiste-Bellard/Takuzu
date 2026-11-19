// SDL2 Demo by aurelien.esnard@u-bordeaux.fr
#include "model.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>  // required to load transparent texture from PNG
#include <SDL2/SDL_ttf.h>    // required to use TTF fonts
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_struct.h"
#include "game_tools.h"


/* **************************************************************** */
#define FONT "../img/arial.ttf"
#define FONTSIZE 36

#define BACKGROUND "../img/background.jpg"
#define ZERO_CASE "../img/ZERO_CASE.png"
#define ONE_CASE "../img/ONE_CASE.png"
#define IMMUTABLE_ZERO_CASE "../img/IMMUTABLE_ZERO_CASE.png"
#define IMMUTABLE_ONE_CASE "../img/IMMUTABLE_ONE_CASE.png"
#define EMPTY_CASE "../img/EMPTY_CASE.png"
#define INTERROGATION "../img/interrogation.png"
#define RESTART "../img/restart.png"
#define SAUVEGARDE "../img/sauvegarde.png"
#define FRONT "../img/front.png"
#define BACK "../img/back.png"
#define SOLVE "../img/solve.png"

/********************************************************************/
struct Env_t {
  SDL_Texture *background;
  SDL_Texture *zero_case;
  SDL_Texture *one_case;
  SDL_Texture *empty_case;
  SDL_Texture *text;
  SDL_Texture *interrogation;
  SDL_Texture *restart;
  SDL_Texture *sauvegarde;
  SDL_Texture *redo;
  SDL_Texture *undo;
  SDL_Texture *solve;
  game g;
};

/* **************************************************************** */

Env *init(SDL_Window *win, SDL_Renderer *ren, game gg) {
  Env *env = malloc(sizeof(struct Env_t));
  /*Loading the different textures into the struct env*/
  env->background = IMG_LoadTexture(ren, BACKGROUND);
  if (!env->background) ERROR("IMG_LoadTexture: %s\n", BACKGROUND);

  env->zero_case = IMG_LoadTexture(ren, ZERO_CASE);
  if (!env->zero_case) ERROR("IMG_LoadTexture: %s\n", ZERO_CASE);

  env->one_case = IMG_LoadTexture(ren, ONE_CASE);
  if (!env->one_case) ERROR("IMG_LoadTexture: %s\n", ONE_CASE);

  env->empty_case = IMG_LoadTexture(ren, EMPTY_CASE);
  if (!env->empty_case) ERROR("IMG_LoadTexture: %s\n", EMPTY_CASE);

  env->interrogation = IMG_LoadTexture(ren, INTERROGATION);
  if (!env->interrogation) ERROR("IMG_LoadTexture: %s\n", INTERROGATION);

  env->restart = IMG_LoadTexture(ren, RESTART);
  if (!env->restart) ERROR("IMG_LoadTexture: %s\n", RESTART);

  env->sauvegarde = IMG_LoadTexture(ren, SAUVEGARDE);
  if (!env->sauvegarde) ERROR("IMG_LoadTexture: %s\n", SAUVEGARDE);

  env->undo = IMG_LoadTexture(ren, BACK);
  if (!env->undo) ERROR("IMG_LoadTexture: %s\n", BACK);

  env->redo = IMG_LoadTexture(ren, FRONT);
  if (!env->redo) ERROR("IMG_LoadTexture: %s\n", FRONT);

  env->solve = IMG_LoadTexture(ren, SOLVE);
  if (!env->solve) ERROR("IMG_LoadTexture: %s\n", SOLVE);

  env->g = gg;
  /*Victory text setting*/
  SDL_Color color = {255, 0, 0, 255};
  TTF_Font *font = TTF_OpenFont(FONT, FONTSIZE);
  if (!font) ERROR("TTF_OpenFont: %s\n", FONT);
  TTF_SetFontStyle(font, TTF_STYLE_BOLD);
  SDL_Surface *surf = TTF_RenderText_Blended(font, "Congratulation !", color);
  env->text = SDL_CreateTextureFromSurface(ren, surf);
  SDL_FreeSurface(surf);
  TTF_CloseFont(font);
  return env;
}
/********************************************************************/
static void getGameDim(int *marginside, int *margin_height,
                       int *cell_size_width, int *cell_size_height, Env *env,
                       SDL_Window *win) {
  /*Dynamic setting of cells and margin based on window size*/
  int w = 0, h = 0;
  SDL_GetWindowSize(win, &w, &h);
  *cell_size_width = (w / env->g->width);
  *cell_size_height = (h / (env->g->height + 1));

  if (*cell_size_height > *cell_size_width) {
    *cell_size_height = *cell_size_width;
  } else if (*cell_size_height < *cell_size_width) {
    *cell_size_width = *cell_size_height;
  }
  *marginside = (w - ((*cell_size_width) * env->g->width)) / 2;
  *margin_height = (h - ((*cell_size_height) * env->g->height)) / 2;
}
/* **************************************************************** */
void render(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  if (win == NULL || env == NULL || ren == NULL) {
    fprintf(stderr, "Null arguments\n");
    exit(EXIT_FAILURE);
  }
  SDL_RenderCopy(ren, env->background, NULL, NULL);
  /*Game Grid Display*/
  int cell_size_width = 0;
  int cell_size_height = 0;
  int margin_width = 0;
  int margin_height = 0;
  getGameDim(&margin_width, &margin_height, &cell_size_width, &cell_size_height,
             env, win);
  for (int i = 0; i < env->g->height; i++) {
    for (int j = 0; j < env->g->width; j++) {
      /*Shows in red the cases where there are errors*/
      if (game_has_error(env->g, i, j)) {
        SDL_SetRenderDrawColor(ren, 255, 0, 0, 255);
        SDL_Rect dst = {.x = margin_width + (j * cell_size_width),
                        .y = (i * cell_size_height),
                        .w = cell_size_width,
                        .h = cell_size_height};
        SDL_RenderFillRect(ren, &dst);
      }
      square s = game_get_square(env->g, i, j);
      SDL_Rect dst = {.x = margin_width + (j * cell_size_width),
                      .y = (i * cell_size_height),
                      .w = cell_size_width,
                      .h = cell_size_height};
      switch (s) {
        case S_IMMUTABLE_ONE:
          SDL_RenderCopy(ren, env->one_case, NULL, &dst);
          break;
        case S_IMMUTABLE_ZERO:
          SDL_RenderCopy(ren, env->zero_case, NULL, &dst);
          break;
        case S_ZERO:
          SDL_RenderCopy(ren, env->zero_case, NULL, &dst);
          break;
        case S_ONE:
          SDL_RenderCopy(ren, env->one_case, NULL, &dst);
          break;
        case S_EMPTY:
          SDL_RenderCopy(ren, env->empty_case, NULL, &dst);
          break;
        default:
          break;
      }
      /*Show victory message if game is win*/
      if (game_is_over(env->g)) {
        SDL_Rect dst = {.x = margin_width,
                        .y = margin_height + cell_size_height,
                        .w = env->g->width * cell_size_width,
                        .h = (env->g->height / 2) * cell_size_height};
        SDL_RenderCopy(ren, env->text, NULL, &dst);
      }
      SDL_SetRenderDrawColor(ren, 102, 205, 0, 255);
      SDL_RenderDrawRect(ren, &dst);
    }
  }
  int button_size = (cell_size_width  * env->g->width) / 6;
  /*img button help*/
  SDL_Rect inte = {.x = margin_width,
                   .y = (env->g->height * cell_size_height),
                   .w = button_size,
                   .h = button_size};
  SDL_RenderCopy(ren, env->interrogation, NULL, &inte);
  /*img button restart*/
  SDL_Rect rest = {.x = margin_width + button_size,
                   .y = (env->g->height * cell_size_height),
                   .w = button_size,
                   .h = button_size};
  SDL_RenderCopy(ren, env->restart, NULL, &rest);
  /*img button sauvegarde*/
  SDL_Rect save = {.x = margin_width + 2*button_size,
                   .y = (env->g->height * cell_size_height),
                   .w = button_size,
                   .h = button_size};
  SDL_RenderCopy(ren, env->sauvegarde, NULL, &save);
  /*img button undo*/
  SDL_Rect und = {.x = margin_width + 3*button_size,
                   .y = (env->g->height * cell_size_height),
                   .w = button_size,
                   .h = button_size};
  SDL_RenderCopy(ren, env->undo, NULL, &und);
  /*img button redo*/
  SDL_Rect red = {.x = margin_width + 4*button_size,
                   .y = (env->g->height * cell_size_height),
                   .w = button_size,
                   .h = button_size};
  SDL_RenderCopy(ren, env->redo, NULL, &red);
  /*img button solve*/
  SDL_Rect sol = {.x = margin_width + 5*button_size,
                   .y = (env->g->height * cell_size_height),
                   .w = button_size,
                   .h = button_size};
  SDL_RenderCopy(ren, env->solve, NULL, &sol);

  SDL_RenderPresent(ren);
}

/* **************************************************************** */

bool process(SDL_Window *win, SDL_Renderer *ren, Env *env, SDL_Event *e) {
  if (e->type == SDL_QUIT) {
    return true;
  }
  /*Plays the moves according to the clicked place and the click*/
  int cell_size_width = 0;
  int cell_size_height = 0;
  int margin_width = 0;
  int margin_height = 0;
  getGameDim(&margin_width, &margin_height, &cell_size_width, &cell_size_height,
             env, win);
  SDL_Point mouse;
  SDL_GetMouseState(&mouse.x, &mouse.y);
  int x = mouse.x;
  int y = mouse.y;
  if (x < margin_width ||
      x > (margin_width + cell_size_width * env->g->width) - 1) {
    return false;
  }
  /*If the mouse is under the game grid, click on the options buttons*/
  if (y < margin_height || y > cell_size_height * env->g->height) {
    int button_size = (cell_size_width  * env->g->width) / 6;
    /*button displayed help*/
    if (x > margin_width && x < margin_width + button_size  &&
        y > (env->g->height * cell_size_height) &&
        y < (env->g->height * cell_size_height) + cell_size_height +
                margin_height) {
      if (e->type == SDL_MOUSEBUTTONDOWN) {
        printf(
           "Object of the game: Fill the grid respecting the rules\n Rules: "
            "Same number of colored squares per row and column, no more than 2"
            "identical squares in a row (vertically or horizontally)\n"
            "Right click to display a white case, Left click for a "
            "show a black case.\n If you place a white case on a "
            "white case, the case is empty, the same for black\n Good"
            "Luck"
            "\n");
      }
    }
    /*restart button*/
    if (x > margin_width + button_size && x < margin_width + 2 * button_size &&
        y > (env->g->height * cell_size_height) && y < (env->g->height * cell_size_height) + cell_size_height) {
      if (e->type == SDL_MOUSEBUTTONDOWN) {
        game_restart(env->g);
      }
    }
    /*save button*/
    if (x > margin_width + 2 * button_size && x < margin_width + 3 * button_size &&
        y > (env->g->height * cell_size_height) && y < (env->g->height * cell_size_height) + cell_size_height) {
      if (e->type == SDL_MOUSEBUTTONDOWN) {
        char file[256] = {0};
        printf("Save name: ");
        scanf(" %s", file);
        game_save(env->g, file);
      }
    }
    /*undo button*/
    if(x > margin_width + 3 * button_size && x < margin_width + 4 * button_size &&
        y > (env->g->height * cell_size_height) && y < (env->g->height * cell_size_height) + cell_size_height){
      if (e->type == SDL_MOUSEBUTTONDOWN){
        game_undo(env->g);
      }
    }

    /*redo button*/
    if(x > margin_width + 4 * button_size && x < margin_width + 5 * button_size &&
        y > (env->g->height * cell_size_height) && y < (env->g->height * cell_size_height) + cell_size_height){
      if (e->type == SDL_MOUSEBUTTONDOWN){
        game_redo(env->g);
      }
    }
    /*solve button*/
    if(x > margin_width + 5 * button_size && x < margin_width + 6 * button_size &&
        y > (env->g->height * cell_size_height) && y < (env->g->height * cell_size_height) + cell_size_height){
      if (e->type == SDL_MOUSEBUTTONDOWN){
        game_solve(env->g);
      }
    }
    return false;
  }
  int i = (y) / cell_size_height;
  int j = (x - margin_width) / cell_size_width;
  if (i >= env->g->height) {
    return false;
  }
  square s = game_get_square(env->g, i, j);
  if (e->type == SDL_MOUSEBUTTONDOWN) {
    if (i >= 0 && i <= env->g->height && j >= 0 && j < env->g->width) {
      /*Left mouse click = zero_case*/
      if (e->button.button == SDL_BUTTON_LEFT) { 
        if (s == S_ZERO) {
          game_play_move(env->g, i, j, S_EMPTY);
        }
        if (s == S_EMPTY || s == S_ONE) {
          game_play_move(env->g, i, j, S_ZERO);
        }
      } 
      /*Right mouse click = one_case*/
      else if (e->button.button ==
                 SDL_BUTTON_RIGHT) { 
        if (s == S_ONE) {
          game_play_move(env->g, i, j, S_EMPTY);
        }
        if (s == S_EMPTY || s == S_ZERO) {
          game_play_move(env->g, i, j, S_ONE);
        }
      }
    }
  }
  return false;
}

/* **************************************************************** */

void clean(SDL_Window *win, SDL_Renderer *ren, Env *env) {
  /*Remove textures*/
  SDL_DestroyTexture(env->background);
  SDL_DestroyTexture(env->zero_case);
  SDL_DestroyTexture(env->one_case);
  SDL_DestroyTexture(env->empty_case);
  SDL_DestroyTexture(env->text);
  SDL_DestroyTexture(env->interrogation);
  SDL_DestroyTexture(env->restart);
  SDL_DestroyTexture(env->sauvegarde);
  game_delete(env->g);
  free(env);
}

/* **************************************************************** */