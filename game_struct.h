/**
 * @file game_struct.h
 * @brief Private Game Structure.
 * @copyright University of Bordeaux. All rights reserved, 2022.
 **/

#ifndef __GAME_STRUCT_H__
#define __GAME_STRUCT_H__

#include <stdbool.h>

#include "game.h"
#include "queue.h"

/* ************************************************************************** */
/*                             DATA TYPES                                     */
/* ************************************************************************** */

/**
 * @brief Game structure.
 * @details This is an opaque data type.
 */
typedef struct game_s {
  uint height;
  uint width;
  square** tab;
  bool wrapping;
  bool unique;
  queue* undo_stack; 
  queue* redo_stack;
} game_s;


/* ************************************************************************** */
/*                                MACRO                                       */
/* ************************************************************************** */

#define INDEX(g, i, j) ((i) * (g->width) + (j))
#define SQUARE(g, i, j) ((g)->tab[(INDEX(g, i, j))])

#endif  // __GAME_STRUCT_H__
