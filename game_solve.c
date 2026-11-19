#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>

#include "game.h"
#include "game_aux.h"
#include "game_ext.h"
#include "game_tools.h"

int main(int argc, char **argv) {
  if(argc != 4 && argc != 3){
    fprintf(stderr,"Not enought parameter ! Please write : ./game_solve <option> <filename> [<filename>]\n");
    exit(EXIT_FAILURE);
  }
  game g = game_load(argv[2]);
  assert(g);

   if (strcmp(argv[1],"-c") == 0) {
    int cpt = game_nb_solutions(g);
    if(argc == 4){
      FILE * output = fopen(argv[3],"w");
      if(output==NULL){
        return EXIT_FAILURE;
      }
      fprintf(output,"%d\n",cpt);
      fclose(output);
      printf("Number of solution :%d\n",cpt);
    }
    else{
      printf("Number of solution :%d\n",cpt);
    }
  } 
  else if (strcmp(argv[1],"-s") == 0) {
    if(game_solve(g) == false){
      printf("No solution!\n");
      game_delete(g);
      return EXIT_FAILURE;
    }else{
      printf("Solution found !\n");
      game_print(g);
      if(argc == 4){
        game_save(g,argv[3]);
      }  
    }
  }
  game_delete(g);
  return EXIT_SUCCESS; 
}