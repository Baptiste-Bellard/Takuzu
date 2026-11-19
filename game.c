#include "game.h"

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_private.h"
#include "game_struct.h"
#include "queue.h"

game game_copy(cgame c_g) {  // Function that make a copy of the game
  if (c_g == NULL) {
    exit(EXIT_FAILURE);
  }
  game g =
      game_new_empty_ext(c_g->height, c_g->width, c_g->wrapping,
                         c_g->unique);  // create & initialize the copied game
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  square tmp;
  for (int i = 0; i < game_nb_rows(c_g); i++) {
    for (int j = 0; j < game_nb_cols(c_g); j++) {
      tmp = game_get_square(
          c_g, i, j);  // getting the squares from the game we want to copy
      game_set_square(g, i, j, tmp);  // setting the squares in the copied game
    }
  }
  return g;
}

bool game_equal(cgame g1, cgame g2) {  // Return TRUE if the two games are the same
  if (g1 == NULL || g2 == NULL) {
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < game_nb_rows(g1); i++) {
    for (int j = 0; j < game_nb_cols(g1); j++) {
      if (game_get_square(g1, i, j) !=
          game_get_square(g2, i, j)) {  // we take a look square by square
        return false;
      }
    }
  }
  return true;
}

void game_set_square(game g, uint i, uint j, square s) {  // Square setter
  if (g == NULL || i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    exit(EXIT_FAILURE);
  }
  g->tab[i][j] = s;
}

int game_get_number(cgame g, uint i,uint j) {  //(Number getter) Function that return the value of the target
                                              // square (EMPTY = -1 ; ZERO = 0 ; ONE = 1)
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    exit(EXIT_FAILURE);
  }
  switch (g->tab[i][j]) {  // use of a switch to have an easier readable code
                           // than a ton of if & else
    case S_EMPTY:
      return -1;
      break;
    case S_ONE:
      return 1;
      break;
    case S_ZERO:
      return 0;
      break;
    case S_IMMUTABLE_ZERO:
      return 0;
      break;
    case S_IMMUTABLE_ONE:
      return 1;
      break;
    default:
      return -1;
  }
}

void game_delete(game g) {  // Function to erase with the game with a
                            // deallocation of the memory
  if (g == NULL) {
    free(g);
    return;
  }
  if (g->tab == NULL) {
    free(g->tab);
    free(g);
    return;
  }
  for (int i = 0; i < game_nb_rows(g); i++) {
    free(g->tab[i]);
  }
  free(g->tab);
  queue_free_full(g->undo_stack, free);
  queue_free_full(g->redo_stack, free);
  free(g);
}

void game_restart(game g) {
  assert(g);

  for (uint i = 0; i < g->height; i++)
    for (uint j = 0; j < g->width; j++) {
      square s = game_get_square(g, i, j);
      if ((s == S_ONE) || (s == S_ZERO)) game_set_square(g, i, j, S_EMPTY);
    }

  // reset history
  _stack_clear(g->undo_stack);
  _stack_clear(g->redo_stack);
}

bool game_is_over(cgame g) {  // Fonction qui renvoie true si le jeu est gagné sinon false
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  // Pour qu'une partie soit finie, aucune case ne doit etre EMPTY. De plus, le
  // jeu ne doit presenter aucune erreur.
  for (int i = 0; i < game_nb_rows(g); i++) {
    for (int j = 0; j < game_nb_cols(g); j++) {
      if (game_has_error(g, i, j) != 0) {
        return false;
      }
      if (game_get_number(g, i, j) == -1) {
        return false;
      }
    }
  }
  return true;
}

void game_play_move(game g, uint i, uint j, square s) {
  assert(g);
  assert(i < g->height);
  assert(j < g->width);

  // check move
  assert(s == S_EMPTY || s == S_ZERO || s == S_ONE);
  bool immutable = game_is_immutable(g, i, j);
  assert(!immutable);

  square cs = game_get_square(g, i, j);  // save current square
  game_set_square(g, i, j, s);           // update with new square

  // save history
  _stack_clear(g->redo_stack);
   move m = {i, j, cs, s};
  _stack_push_move(g->undo_stack, m);
}

bool game_check_move(cgame g, uint i, uint j,square s) {  // Fonction qui renvoie true si on peut jouer le coup aux
                                                           // coordonnées i,j et placer le square s
  if (g == NULL || i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    return false;
  } else if (game_is_immutable(g, i, j)) {
    return false;
  } else if ((s != S_EMPTY) && (s != S_ONE) && (s != S_ZERO)) {
    return false;
  } else {
    return true;
  }
}

int game_has_error(cgame g, uint i, uint j)
{
  assert(g);
  assert(i < g->height);
  assert(j < g->width);

  int num = game_get_number(g, i, j);

  // no error occurs if the square is empty
  if (num == -1) return 0;

  // check number of ones and zeros in row i
  uint nb_zeros = _count_in_row(g, 0, i);
  uint nb_ones = _count_in_row(g, 1, i);
  if ((num == 0) && (nb_zeros > (g->width / 2))) return 1;
  if ((num == 1) && (nb_ones > (g->width / 2))) return 1;

  // check number of ones and zeros in col j
  nb_zeros = _count_in_col(g, 0, j);
  nb_ones = _count_in_col(g, 1, j);
  if ((num == 0) && (nb_zeros > (g->width / 2))) return 2;
  if ((num == 1) && (nb_ones > (g->width / 2))) return 2;

  // check three consecutive in row i
  bool cons_row = _three_consecutive_row(g, i, j);
  if (cons_row) return 3;

  // check three consecutive in col j
  bool cons_col = _three_consecutive_col(g, i, j);
  if (cons_col) return 4;

  if (g->unique) {
    // check row i is unique
    bool uniq_row = _unique_row(g, i, false);
    if (!uniq_row) return 5;

    // check col j is unique
    bool uniq_col = _unique_col(g, j, false);
    if (!uniq_col) return 6;
  }

  return 0;
}


bool game_is_immutable(cgame g, uint i,uint j) {  // Fonction qui renvoie true si le square aux coordonnées i,j est
               // immutable ou non sino false
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_rows(g)) {
    exit(EXIT_FAILURE);
  }
  if (g->tab[i][j] == S_IMMUTABLE_ZERO || g->tab[i][j] == S_IMMUTABLE_ONE) {
    return true;
  }
  return false;
}

bool game_is_empty(cgame g, uint i,uint j) {  // Fonction qui renvoie true sur la case aux
                                              // coordonnées i,j est vide sinon false
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    exit(EXIT_FAILURE);
  }
  return g->tab[i][j] == S_EMPTY;
}

/*FONCTION BBELLARD V2 CHECK */

int game_get_next_number(cgame g, uint i, uint j, direction dir,uint dist) {  // Fonction qui retourne le numéro de la case à une distance
                  // dist dans une direction dir de la case i,j
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g) || dist > 2) {
    exit(EXIT_FAILURE);
  }
  int i2 = i;
  int j2 = j;
  uint c = game_nb_cols(g);
  uint r = game_nb_rows(g);

  if (game_is_wrapping(g)) {
    if (dir == LEFT)
      j2 = ((j2 - dist + c) % c);
    else if (dir == RIGHT)
      j2 = ((j2 + dist + c) % c);
    else if (dir == UP)
      i2 = ((i2 - dist + r) % r);
    else
      i2 = ((i2 + dist + r) % r);
  }
  switch (dir) {
    case LEFT:
      j2 = j2 - dist;
      if (j2 < 0) {
        return -1;
      }
      break;
    case RIGHT:
      j2 = j2 + dist;
      if (j2 >= g->width) {
        return -1;
      }
      break;
    case UP:
      i2 = i2 - dist;
      if (i2 < 0) {
        return -1;
      }
      break;
    case DOWN:
      i2 = i2 + dist;
      if (i2 >= g->height) {
        return -1;
      }
      break;
  }
  return game_get_number(g, i2, j2);
}

int game_get_next_square(cgame g, uint i, uint j, direction dir,uint dist) {  // Fonction qui retourne le sqaure de la case à une distance
                  // dist dans une direction dir de la case i,j
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g) || dist > 2) {
    exit(EXIT_FAILURE);
  }
  int i2 = i;
  int j2 = j;
  uint c = game_nb_cols(g);
  uint r = game_nb_rows(g);

  if (game_is_wrapping(g)) {
    if (dir == LEFT)
      j2 = ((j2 - dist + c) % c);
    else if (dir == RIGHT)
      j2 = ((j2 + dist + c) % c);
    else if (dir == UP)
      i2 = ((i2 - dist + r) % r);
    else
      i2 = ((i2 + dist + r) % r);
  }
  switch (dir) {
    case LEFT:
      j2 = j2 - dist;
      if (j2 < 0) {
        return -1;
      }
      break;
    case RIGHT:
      j2 = j2 + dist;
      if (j2 >= g->width) {
        return -1;
      }
      break;
    case UP:
      i2 = i2 - dist;
      if (i2 < 0) {
        return -1;
      }
      break;
    case DOWN:
      i2 = i2 + dist;
      if (i2 >= g->height) {
        return -1;
      }
      break;
  }
  return game_get_square(g, i2, j2);
}

square game_get_square(cgame g, uint i,uint j) {  // Fonction qui retourne le square de la case i,j
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  if (i >= game_nb_rows(g) || j >= game_nb_cols(g)) {
    exit(EXIT_FAILURE);
  }
  return g->tab[i][j];
}

game game_new_empty(void) {  // Fonction qui créee une game vide
  game G1 = (game)malloc(sizeof(struct game_s));
  if (G1 == NULL) {
    exit(EXIT_FAILURE);
  }
  G1->height = DEFAULT_SIZE;
  G1->width = DEFAULT_SIZE;

  G1->tab = (square**)malloc(G1->height * sizeof(square*));
  if (G1->tab == NULL) {
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < G1->height; i++) {
    G1->tab[i] = (square*)malloc(G1->width * sizeof(square));
    for (int j = 0; j < G1->width; j++) {
      G1->tab[i][j] = S_EMPTY;
    }
  }

  return G1;
}

game game_new(square* squares) {  // Fonction qui créee une game avec les squares données
  if (squares == NULL) {
    exit(EXIT_FAILURE);
  }
  game g = game_new_ext(DEFAULT_SIZE, DEFAULT_SIZE, squares, false, false);
  if (g == NULL) {
    exit(EXIT_FAILURE);
  }
  for (int i = 0; i < g->height; i++) {
    for (int j = 0; j < g->width; j++) {
      g->tab[i][j] = squares[i * DEFAULT_SIZE + j];
    }
  }
  return g;
}