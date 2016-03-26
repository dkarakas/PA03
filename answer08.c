#include <stdio.h>
#include <stdlib.h>
#include "answer08.h"

// if you want to declare and define new functions, do not name them
// with a prefix "__".  The instructors will be using functions with
// such names to test your individual functions.  You should not be 
// calling a function whose name has a prefix "__" because your program
// should compile without any functions from the instructors.
// you may put your additional functions here or at the end of this file

char get_loc_type(FILE* fptr, int row, int col){
  fseek(fptr,0,SEEK_SET);
  char ch;
  char char_ret;
  int cur_row = 0;
  if(row == 0){
    fseek(fptr,col,SEEK_CUR);
    return fgetc(fptr);
  }
  else{
    while((ch=fgetc(fptr))!=EOF){
      if(ch=='\n')
         cur_row++;
      if(cur_row == row){
        fseek(fptr,col,SEEK_CUR);
        char_ret = fgetc(fptr);
        fseek(fptr,0,SEEK_END);
      }
    }
  }
  return char_ret;
}

// allocate space from heap memory for the user-defined structure Maze, and
// allocate nrow x ncol maze and store the address in the structure (in 
// maze_array)
// also assign nrow and ncol fields in the structure accordingly
// if allocation fails, return NULL
// if allocation fails, you are also responsible for freeing the memory
// allocated in this function before the failure
// may assume that nrow and ncol are > 0

Maze *Allocate_maze_space(int nrow, int ncol)
{
  Maze *pointerMaze;
  pointerMaze = malloc(sizeof(Maze));
  if(pointerMaze == NULL)
    return NULL;
  else{
    pointerMaze->nrow = nrow;
    pointerMaze->ncol = ncol;
    pointerMaze->maze_array = malloc(sizeof(char *) * nrow);
    if(pointerMaze->maze_array == NULL){
      free(pointerMaze);
      return NULL;
    }
    int i;
    for(i=0;i<nrow;i++){
      pointerMaze->maze_array[i] = malloc(sizeof(char) * ncol);
      if(pointerMaze->maze_array[i] == NULL){
        int j;
	for(j = i; j>=0; j--)
	  free(pointerMaze->maze_array[j]);
        free(pointerMaze->maze_array);
	free(pointerMaze);
	return NULL;	  
      }  
    }
  }
  return pointerMaze;    
}

// free the memory used for the maze
// you may assume that maze is not NULL, and all memory addresses are valid

void Deallocate_maze_space(Maze *maze)
{
   int i;
   if(maze == NULL)
     return;
   for(i=0; i < (maze->nrow);i++)
     free(maze->maze_array[i]);
   free(maze->maze_array);
   free(maze);   
   return;
}

/* Read maze in a multi-line file and store the necessary information */
/* into a 2D array of characters.  The address of the 2D array and the */
/* dimensions of the 2D array should be stored in a Maze structure allocated */
/* from the heap memory */
/* if the allocation fails, should return NULL */

Maze *Read_maze_from_2Dfile(FILE *fptr)
{
  fseek(fptr,0,SEEK_SET);
  int nrow = 0;
  int ncol = 0;
  char ch;
  while((ch=fgetc(fptr)) != EOF){
    if(ch=='\n')
      nrow++;
    if(nrow==0)
      ncol++;
  }
  Maze *pointerMaze = Allocate_maze_space(nrow,ncol);
  if(pointerMaze == NULL){
    Deallocate_maze_space(pointerMaze);
    return NULL;  
  }
  else{ 
    int i;   
    int j;
    fseek(fptr,0,SEEK_SET);
    for(i = 0; i < nrow; i++){
      for(j = 0; j < ncol; j++){
        ch = get_loc_type(fptr,i,j);
          pointerMaze->maze_array[i][j] = ch;
      }
    } 
  }
  return pointerMaze;
}

/* Write the maze into a multi-line file */
/* the maze has to stay intact */

int Write_maze_to_2Dfile(char *filename, const Maze *maze)
{
  int wordCount = 0;
  FILE* outputFILE;
  outputFILE = fopen(filename, "w");
  int i;
  int j;
  for(i = 0; i < maze->nrow; i++){
    for(j = 0; j < (maze->ncol+1); j++){
      if(maze->ncol != maze->ncol)
        fprintf(outputFILE,"%c",maze->maze_array[i][j]);
      else
        fprintf(outputFILE,"\n");
      wordCount++;
    }
  }
  fclose(outputFILE);
  return wordCount;
}

/* Expand the maze from nrow x ncol to (2nrow - 1) x ncol */
/* the top half of the maze is the same as the original maze */
/* the bottom half of the maze is a reflection of the original maze */
/* You have to allocate space for a new maze */
/* the nrow and ncol fields of the new maze have to be assigned accordingly */
/* if you can't expand the maze because of memory issue, NULL should be */
/* returned */
/* the original maze has to stay intact */

Maze *Expand_maze_row(const Maze *maze)
{
  Maze *newMaze = Allocate_maze_space( (maze->nrow*2-1), maze->ncol);
  if(newMaze == NULL)
    return NULL;
  else{
    newMaze->nrow = (maze->nrow*2-1);
    newMaze->ncol = maze->ncol;
    //fprintf(stderr,"\nnew nrow %d new ncol %d.... nrow %d ncol %d\n ", newMaze->nrow,newMaze->ncol, maze->nrow, maze->ncol);
    int row;
    int col;
    int newRow= 0;
    for(row = 0; row < (maze->nrow); row++){  
      for(col = 0; col < maze->ncol; col++){
        newMaze->maze_array[newRow][col] = maze->maze_array[row][col]; 
        //fprintf(stderr, "\n %d %d %d", newRow,row,col);
      }
    newRow++;
    }
    for(row = (maze->nrow - 2); row >= 0 ; row--){
      for(col = 0; col < maze->ncol; col++){
       // fprintf(stderr,"\n  %d \n", row);
        newMaze->maze_array[newRow][col] = maze->maze_array[row][col]; 
      }
    newRow++;
    }
  }
  return newMaze;
}

/* Expand the maze from nrow x ncol to nrow x 2ncol - 1 */
/* the left half of the maze is the same as the original maze */
/* the right half of the maze is a reflection of the original maze */
/* moreover, you have to create an opening between the left and right halves */
/* the opening should be at the middle row of the maze */
/* the opening must also be accessible from one of the paths in the */
/* new maze */
/* to do that, you have to convert some locations that are WALL to PATH */
/* starting from the new opening to the left and to the right until you reach */
/* a location that is adjacent to a location that is PATH */
/* You have to allocate space for a new maze */
/* the nrow and ncol fields of the new maze have to be assigned accordingly */
/* if you can't expand the maze because of memory issue, NULL should be */
/* returned */
/* the original maze has to stay intact */

Maze *Expand_maze_column(const Maze *maze)
{
  Maze *newMaze = Allocate_maze_space( maze->nrow, (maze->ncol*2-1));
  if(newMaze == NULL)
    return NULL;
  else{
    newMaze->nrow = maze->nrow;
    newMaze->ncol = (maze->ncol*2-1);
    //fprintf(stderr,"\n 2 - new nrow %d new ncol %d.... nrow %d ncol %d\n ", newMaze->nrow,newMaze->ncol, maze->nrow, maze->ncol);
    int row;
    int col;
    int curPath=1;
    int newCol = maze->ncol-1;
    for(row = 0; row < maze->nrow; row++){  
      for(col = 0; col < (maze->ncol-1); col++){
        newMaze->maze_array[row][col] = maze->maze_array[row][col]; 
      }
    }
    for(row = 0 ; row < maze->nrow ; row++){
      for(col = (maze->ncol-1); col >= 0; col--){
        newMaze->maze_array[row][newCol] = maze->maze_array[row][col]; 
        newCol++;
	//printf("%d %d ||",newCol, col);
	if(col==0)
	  newMaze->maze_array[row][newCol+0]='\0';
      }
    newCol = maze->ncol-1;
    //printf("\n");
    }
    
    newMaze->maze_array[maze->nrow/2][maze->ncol - 1] = ' ';
    
    while(newMaze->maze_array[maze->nrow/2+1][maze->ncol-1+curPath-1] != ' ' &&
          newMaze->maze_array[maze->nrow/2-1][maze->ncol-1+curPath-1] != ' ' &&
          newMaze->maze_array[maze->nrow/2][maze->ncol-1+curPath] != ' ' &&
          newMaze->maze_array[maze->nrow/2][maze->ncol-1-curPath] != ' '){
        
          newMaze->maze_array[maze->nrow/2][maze->ncol-1-curPath] = ' ';
          newMaze->maze_array[maze->nrow/2][maze->ncol-1+curPath] = ' ';
          curPath++;
    }
  }
  
  return newMaze;
}

// if you want to declare and define new functions, put them here
// or at the beginning of the file

