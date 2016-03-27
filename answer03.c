#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include "answer03.h"

//inserted functions from previous pes.
char get_loc_type(FILE* fptr, int row, int col);			//used
Maze *Allocate_maze_space(int nrow, int ncol);				//used
void Deallocate_maze_space(Maze *maze);					//used
Maze *Read_maze_from_2Dfile(FILE *fptr);				//used
int Write_maze_to_2Dfile(char *filename, const Maze *maze);		//used
void Find_maze_dimensions(FILE *fptr, int *nrow, int *ncol);		//used

//linked list for shortest path
typedef struct _nodePath{
  char ch;
  struct _nodePath *nextChar;
}nodePath;

nodePath *ln_construct(char ch);
nodePath *ln_insert(nodePath **current_list , char ch);
void ln_path_destroy(nodePath *head);
nodePath *rmv_one_step(nodePath **list);
nodePath *ln_copy(nodePath *current_list);


/* given a maze, given a location cur, is that PATH */
/* if out of bound, return 0                        */
/* if within bound, but not PATH, return 0          */
/* otherwise return 1                               */

static int Is_path(Maze *maze, int row, int col)
{
   if ((row >= 0) && (row < maze->nrow)) {
      if ((col >= 0) && (col < maze->ncol)) {
         return maze->maze_array[row][col] == PATH;
      }
   }
   return 0;
}

static void Pathfinder_helper_dimcho(Maze *maze, int curr, int curc, int endr, int endc,
                            int count, nodePath* shortest_path, nodePath* tmp_shortest_path, int nrow, int ncol, int* cur_count,char rem)
{
   maze->maze_array[curr][curc] = VISITED;
   fprintf(stderr,"(%d, %d, %d), %d\n", (curr+1), (curc+1), *cur_count, count);
   if ((curr == endr) && (curc == endc)) { // reach the destination 
      if(*cur_count >  count){
        fprintf(stderr,"NEW MORE EFFICIENT PATH FOUND ");
        *cur_count = count;

   printf("\n\n\n\n\n\n\n  %d ", *cur_count);   
   printf("   ");   
   nodePath* print= tmp_shortest_path;
   while (print != NULL) {
      printf("%c->", print->ch);
      print = print->nextChar;
   }
   printf("NULL\n");
   
        ln_path_destroy(shortest_path);
        shortest_path = ln_copy(tmp_shortest_path);

      } 
      tmp_shortest_path = rmv_one_step(&tmp_shortest_path);
   }

//if(count == 40)
//  exit(10);
   if(curr < 0 || curc < 0 || endr < 0 || endc < 0 ||
      curr >= nrow || curc >=ncol || endr >= nrow || endc >= ncol){
      fprintf(stderr,"RECHED OUT OF BOUNDS\n");
   }
   if (Is_path(maze, curr-1, curc) && rem != 'N') {
      tmp_shortest_path = ln_insert(&tmp_shortest_path, 'N');
      Pathfinder_helper_dimcho(maze, curr-1, curc, endr, endc, count+1, shortest_path,tmp_shortest_path,nrow,ncol,cur_count,'S');
      tmp_shortest_path = rmv_one_step(&tmp_shortest_path);
   }
   if (Is_path(maze, curr+1, curc) && rem != 'S') {
      tmp_shortest_path = ln_insert(&tmp_shortest_path, 'S');
      Pathfinder_helper_dimcho(maze, curr+1, curc, endr, endc, count+1, shortest_path,tmp_shortest_path,nrow,ncol,cur_count,'N');
      tmp_shortest_path = rmv_one_step(&tmp_shortest_path);
   }
   if (Is_path(maze, curr, curc+1) && rem != 'E') {
      tmp_shortest_path = ln_insert(&tmp_shortest_path, 'E');
      Pathfinder_helper_dimcho(maze, curr, curc+1, endr, endc, count+1, shortest_path,tmp_shortest_path,nrow,ncol,cur_count,'W');
      tmp_shortest_path = rmv_one_step(&tmp_shortest_path);
   }
   if (Is_path(maze, curr, curc-1) && rem != 'W') {
      tmp_shortest_path = ln_insert(&tmp_shortest_path, 'W');
      Pathfinder_helper_dimcho(maze, curr, curc-1, endr, endc, count+1, shortest_path,tmp_shortest_path,nrow,ncol,cur_count,'E');
      tmp_shortest_path = rmv_one_step(&tmp_shortest_path);
   }
//   printf("   ");   
//   nodePath* print= shortest_path;
//   while (print != NULL) {
//      printf("%c->", print->ch);
//      print = print->nextChar;
//   }
//   printf("NULL\n");
   
   maze->maze_array[curr][curc] = PATH;
}

/* given a maze, current row (curr),                                     */
/* current column (curc), destination row (drow), destination col (dcol) */
/* current distance from source, determine the where to explore from the */
/* current location                                                      */
/* a recursive function: from the current location, try 'N', 'S', 'E',   */
/* 'W' locations one at a time if the attempt is successful, write the   */
/* direction in the file                                                 */

static int Pathfinder_helper(Maze *maze, int curr, int curc, int endr, int endc,
                            int count, FILE *dfptr)
{
   // you may uncomment this and use the output here to trace the function
   // with sample5.pdf
   fprintf(stderr,"(%d, %d), %d\n", curr, curc, count);
   maze->maze_array[curr][curc] = VISITED;
   if ((curr == endr) && (curc == endc)) { // reach the destination 
      int i;
      for (i = 0; i < count; i++) { // create enough space in file
         fputc(VISITED, dfptr);
      }
      return count;
   }

   int found;
   if (Is_path(maze, curr-1, curc)) {
      found = Pathfinder_helper(maze, curr-1, curc, endr, endc, count+1, dfptr);
      if (found != -1) {
         fseek(dfptr, -1, SEEK_CUR); // go back one position
         fputc('N', dfptr);
         fseek(dfptr, -1, SEEK_CUR); // go back one position
         return found;
      }
   }
   if (Is_path(maze, curr+1, curc)) {
      found = Pathfinder_helper(maze, curr+1, curc, endr, endc, count+1, dfptr);
      if (found != -1) {
         fseek(dfptr, -1, SEEK_CUR); // go back one position
         fputc('S', dfptr);
         fseek(dfptr, -1, SEEK_CUR); // go back one position
         return found;
      }
   }
   if (Is_path(maze, curr, curc+1)) {
      found = Pathfinder_helper(maze, curr, curc+1, endr, endc, count+1, dfptr);
      if (found != -1) {
         fseek(dfptr, -1, SEEK_CUR); // go back one position
         fputc('E', dfptr);
         fseek(dfptr, -1, SEEK_CUR); // go back one position
         return found;
      }
   }
   if (Is_path(maze, curr, curc-1)) {
      found = Pathfinder_helper(maze, curr, curc-1, endr, endc, count+1, dfptr);
      if (found != -1) {
         fseek(dfptr, -1, SEEK_CUR); // go back one position
         fputc('W', dfptr);
         fseek(dfptr, -1, SEEK_CUR); // go back one position
         return found;
      }
   }
   return -1;
}

/* this is a function implemented by the instructor to illustrate           */
/* how to use a recursive function to go from the left-most top entrance to */
/* the right-most bottom exit of a maze.  The function will print the       */
/* directions taken to reach the bottom exit, without visiting a location   */
/* twice.  Note that the recursion may visit a location a few times         */
/* Given three filenames, first one is a given maze, the second one is to   */
/* be written with the directions, the third is the explored maze           */
/* If the maze file does not exist, the function immediately return -1 and  */
/* do nothing else                                                          */
/* the directions are in the form of a series of characters                 */
/* 'N', 'S', 'E', 'W', without spacing (and without the quotes).            */
/* There should be just a single line, without a terminating newline        */
/* character. In other words, to the vi editor, it is an incomplete file    */
/* you are to assume that you start from the top-most, left-most entry      */
/* of the maze.  Therefore, the first direction is always 'S'               */
/* similarly, the last direction is always 'S'                              */
/* the returned value should be the number of steps taken to reach          */
/* the right-most bottom exit.  It is typically not the shortest distance   */
/* Note that the maze can be generated by amaze, or some combinations of    */
/* the maze expansion functions that you have written in PE08               */
/* you may assume that the maze file, if it exists, is in the correct format*/

int Find_path_from_top_entrance_to_bottom_exit(char *mazefile, char *directionfile, char *visitedfile)
{
   FILE *mfptr = fopen(mazefile, "r");
   if (mfptr == NULL) {
      return -1;
   }
   
   /* read the maze from mfptr */

   Maze *maze = Read_maze_from_2Dfile(mfptr);
   fclose(mfptr);
   if (maze == NULL) {
      return -1;
   }
   
   /* find the starting location, the top-most, left-most opening */
   int startr, startc;
   startr = 0;
   for (startc = 0; startc < maze->ncol; startc++) {
      if (maze->maze_array[startr][startc] == PATH) {
         break;
      }
   }

   /* find the ending location, the bottom-most, right most opening */
   int endr, endc;
   endr = maze->nrow-1;
   for (endc = maze->ncol-1; endc >= 0; endc--) {
      if (maze->maze_array[endr][endc] == PATH) {
         break;
      }
   } 

   // no starting location, deallocate and return 
   if (startc == maze->ncol) {
      Deallocate_maze_space(maze);
      return -1;
   }
   // no ending location, deallocate and return 
   if (endc == -1) {
      Deallocate_maze_space(maze);
      return -1;
   }

   FILE *dfptr = fopen(directionfile, "w");
   if (dfptr == NULL) {
      Deallocate_maze_space(maze);
      return -1;
   }
   int steps = Pathfinder_helper(maze, startr, startc, endr, endc, 0, dfptr);
   Write_maze_to_2Dfile(visitedfile, maze);
   fclose(dfptr);
   Deallocate_maze_space(maze);
   return steps;
}

/* this function expects four parameters: two filenames, and coordinates */
/* of two locations                                                      */
/* two filenames: first one is a given maze, the second one is to        */
/* be written with the directions                                        */
/* coordinates of two locations: source and destination locations        */
/*                                                                       */
/* If the maze file does not exist, you should immediately return -1 and */
/* do nothing else, i.e., the direction file should not be created       */
/* If the maze file cannot be read into a Maze structure, you should     */
/* immediately return -1 and do nothing else                             */
/* If the coordinates are not valid, do nothing and return -1            */
/* If the directions file cannot be opened for writing, return -1        */
/* The function should write the directions of a shortest path from the  */
/* source location to the destination location into the directions file  */
/* the directions are in the form of a series of characters              */
/* 'N', 'S', 'E', 'W', without spacing (and without the quotes).         */
/* There should be just a single line, without a terminating newline     */
/* character. In other words, to the vi editor, it is an incomplete file */
/* The directions should allow an agent to travel in the maze from the   */
/* source coordinates to the destination coordinates using the fewest    */
/* number of moves                                                       */
/* the number of moves taken by the agent in a shortest path should be   */
/* returned                                                              */
/* Note that the maze can be generated by amaze, or some combinations of */
/* the maze expansion functions that you have written in PE08            */
/* you may assume that the maze file, if it exists, is in the correct    */
/* format                                                                */
int DFS_shortest_path_directions(char *mazefile, char *directionfile,
                        Coord source, Coord destination)
{
  //VARIABLES
  FILE *fptr_mazefile; 
  FILE *fptr_directionfile;
  int nrow, ncol;
  Maze *maze_array_input;
  //char ch;
  //int cur_row, cur_col;
  
  nodePath *shortest_path;
  nodePath *tmp_shortest_path;
  int counter = 0;
  int cur_count = 32767;
  int loc_visited = 1;

  //ACTUAL CODE
  if(mazefile == NULL || directionfile == NULL ){
    fprintf(stderr,"Either mazefile or directionfile or was not given");
    return -1;
  }

  //openning the file
  fptr_mazefile = fopen(mazefile,"r");
  if(fptr_mazefile == NULL){
    fprintf(stderr,"Can't open mazefile for reading! ");
    return -1;
  }
  
  //finding dimensions and allocating space for the array
  //Then putting the file into the array
  
  Find_maze_dimensions(fptr_mazefile, &nrow, &ncol);
  maze_array_input = Read_maze_from_2Dfile(fptr_mazefile);
  if(maze_array_input == NULL){
    fclose(fptr_mazefile);
    fprintf(stderr,"Can't allocate enough memory for maze");
    return -1;
  }
  
  //Checking if source or destination are valid.
  if( nrow <= source.row || ncol <= source.col ||
      nrow <= destination.row || ncol <= destination.col ||
      source.row < 0 || source.col < 0 ||
      destination.row < 0 || destination.col<0 ||
      (get_loc_type(fptr_mazefile, source.row, source.col) != PATH) ||
      (get_loc_type(fptr_mazefile, destination.row, destination.col) != PATH)){
    fclose(fptr_mazefile);
    Deallocate_maze_space(maze_array_input);
    fprintf(stderr,"Invalid source or destination\n");
    return -1;
  }
  
  //openning directional file
  fptr_directionfile = fopen(directionfile,"w");
  if(fptr_directionfile == NULL){
    fclose(fptr_mazefile);
    Deallocate_maze_space(maze_array_input);
    fprintf(stderr,"Can't open directional file for writing ");
    return -1;
  }
  
  shortest_path = ln_construct('Q');
  tmp_shortest_path = ln_construct('Q');

  Pathfinder_helper_dimcho(maze_array_input, source.row, source.col, destination.row, destination.col, counter, shortest_path, tmp_shortest_path, nrow, ncol, &cur_count,'I');
  /*Write_maze_to_2Dfile(directionfile, maze_array_input);*/
//  nodePath* print= shortest_path;
//   while (print != NULL) {
//      printf("%c->", print->ch);
//      print = print->nextChar;
//   }
   printf("NULL\n");


  ln_path_destroy(shortest_path);
  ln_path_destroy(tmp_shortest_path);
  fclose(fptr_directionfile);
  fclose(fptr_mazefile);
  Deallocate_maze_space(maze_array_input);
  return loc_visited;
}

int Simulate_movement(char *mazefile, char *directionfile, char *visitedfile,
                      Coord source, Coord destination)
{
  //VARIABLES
  FILE *fptr_mazefile; 
  FILE *fptr_directionfile;
  int nrow, ncol;
  Maze *maze_array_input;
  char ch;
  int cur_row, cur_col;
  int loc_visited = 1;
  //ACTUAL CODE
  if(mazefile == NULL || directionfile == NULL || visitedfile == NULL){
    fprintf(stderr,"Either mazefile or directionfile or visitedfile was not given");
    return -1;
  }

  //openning the file
  fptr_mazefile = fopen(mazefile,"r");
  if(fptr_mazefile == NULL){
    fprintf(stderr,"Can't open mazefile for reading ");
    return -1;
  }
  
  //finding dimensions and allocating space for the array
  //Then putting the file into the array
  
  Find_maze_dimensions(fptr_mazefile, &nrow, &ncol);
  //maze_array_input = Allocate_maze_space(nrow, ncol);
  maze_array_input = Read_maze_from_2Dfile(fptr_mazefile);
  if(maze_array_input == NULL){
    fclose(fptr_mazefile);
    fprintf(stderr,"Can't allocate enough memory for maze");
    return -1;
  }

  //openning dierectional file
  fptr_directionfile = fopen(directionfile,"r");
  if(fptr_directionfile == NULL){
    fclose(fptr_mazefile);
    Deallocate_maze_space(maze_array_input);
    fprintf(stderr,"Can't open mazefile for reading ");
    return -1;
  }
  
  //Checking if source or destination are valid.
  if( nrow <= source.row || ncol <= source.col ||
      nrow <= destination.row || ncol <= destination.col ||
      source.row < 0 || source.col < 0 ||
      destination.row < 0 || destination.col<0 ||
      (get_loc_type(fptr_mazefile, source.row, source.col) != PATH) ||
      (get_loc_type(fptr_mazefile, destination.row, destination.col) != PATH)){
    fclose(fptr_directionfile);
    fclose(fptr_mazefile);
    Deallocate_maze_space(maze_array_input);
    fprintf(stderr,"Invalid source or destination\n");
    return -1;
  }
 
  //visiting locations 
  fseek(fptr_directionfile,0,SEEK_SET);
  cur_row = source.row;
  cur_col = source.col;
  maze_array_input->maze_array[cur_row][cur_col] = VISITED;   
  //north (up), south (down), east (right), west (left)
  while((ch = getc(fptr_directionfile)) != EOF){
     if(ch == 'N'){
       cur_row--;
     
       if( nrow <= cur_row || ncol <= cur_col || (cur_row < 0) || (cur_col < 0)){
         Write_maze_to_2Dfile(visitedfile,maze_array_input);
         fclose(fptr_directionfile);
         fclose(fptr_mazefile);
         Deallocate_maze_space(maze_array_input);
         fprintf(stderr,"Direction file brought us out of bounds");
         return -1;
       }

       if(maze_array_input->maze_array[cur_row][cur_col] == WALL){
         Write_maze_to_2Dfile(visitedfile,maze_array_input);
         fclose(fptr_directionfile);
         fclose(fptr_mazefile);
         Deallocate_maze_space(maze_array_input);
         fprintf(stderr,"We hit wall!");
         return -1;
       }
       
        maze_array_input->maze_array[cur_row][cur_col] = VISITED; 

     } else if(ch == 'S'){
       cur_row++;
       //fprintf(stderr,"cur_row %d cur_col %d", cur_row, cur_col);
       if( (nrow <= cur_row) || (ncol <= cur_col) || (cur_row < 0) || (cur_col < 0)){
         Write_maze_to_2Dfile(visitedfile,maze_array_input);
         fclose(fptr_directionfile);
         fclose(fptr_mazefile);
         Deallocate_maze_space(maze_array_input);
         fprintf(stderr,"Direction file brought us out of bounds");
         return -1;
       }

       if(maze_array_input->maze_array[cur_row][cur_col] == WALL){
         Write_maze_to_2Dfile(visitedfile,maze_array_input);
         fclose(fptr_directionfile);
         fclose(fptr_mazefile);
         Deallocate_maze_space(maze_array_input);
         fprintf(stderr,"We hit wall!");
         return -1;
       }

        maze_array_input->maze_array[cur_row][cur_col] = VISITED; 
     
     }else if(ch == 'E'){
       cur_col++;
       
       if( nrow <= cur_row || ncol <= cur_col || (cur_row < 0) || (cur_col < 0)){
         Write_maze_to_2Dfile(visitedfile,maze_array_input);
         fclose(fptr_directionfile);
         fclose(fptr_mazefile);
         Deallocate_maze_space(maze_array_input);
         fprintf(stderr,"Direction file brought us out of bounds");
         return -1;
       }
     
       if(maze_array_input->maze_array[cur_row][cur_col] == WALL){
         Write_maze_to_2Dfile(visitedfile,maze_array_input);
         fclose(fptr_directionfile);
         fclose(fptr_mazefile);
         Deallocate_maze_space(maze_array_input);
         fprintf(stderr,"We hit wall!");
         return -1;
       }

        maze_array_input->maze_array[cur_row][cur_col] = VISITED; 
     
     }else if(ch == 'W'){
       cur_col--;

       if( nrow <= cur_row || ncol <= cur_col || (cur_row < 0) || (cur_col < 0)){
         Write_maze_to_2Dfile(visitedfile,maze_array_input);
         fclose(fptr_directionfile);
         fclose(fptr_mazefile);
         Deallocate_maze_space(maze_array_input);
         fprintf(stderr,"Direction file brought us out of bounds");
         return -1;
       }
     
       if(maze_array_input->maze_array[cur_row][cur_col] == WALL){
         Write_maze_to_2Dfile(visitedfile,maze_array_input);
         fclose(fptr_directionfile);
         fclose(fptr_mazefile);
         Deallocate_maze_space(maze_array_input);
         fprintf(stderr,"We hit wall!");
         return -1;
       }

        maze_array_input->maze_array[cur_row][cur_col] = VISITED; 

     }else{
       Write_maze_to_2Dfile(visitedfile,maze_array_input);
       fclose(fptr_directionfile);
       fclose(fptr_mazefile);
       Deallocate_maze_space(maze_array_input);
       fprintf(stderr,"Reached unvalid next location symbol in direction file");
       return -1;
     }
    loc_visited++;
  }
  
  if(cur_row == destination.row && cur_col == destination.col){
         fprintf(stderr,"are %c the %c end %c \n",maze_array_input->maze_array[0][0],maze_array_input->maze_array[0][1],maze_array_input->maze_array[1][5]);
    Write_maze_to_2Dfile(visitedfile,maze_array_input);
    fclose(fptr_directionfile);
    fclose(fptr_mazefile);
    Deallocate_maze_space(maze_array_input);
    return loc_visited;
  }else{
    fprintf(stderr,"We haven't reached the destination");
    Write_maze_to_2Dfile(visitedfile,maze_array_input);
    fclose(fptr_directionfile);
    fclose(fptr_mazefile);
    Deallocate_maze_space(maze_array_input);
    return -1;
  }
}




// Inserted functions from previous pes.

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

int Write_maze_to_2Dfile(char *filename, const Maze *maze)
{
  int wordCount = 0;
  FILE* outputFILE;
  //int printed_char;
  outputFILE = fopen(filename, "w+");
  if(outputFILE == NULL){
    fprintf(stderr,"There is a problem opening the visitedfile");
    return -1;
  }
  int i;
  int j;
  for(i = 0; i < maze->nrow; i++){
    for(j = 0; j < (maze->ncol+1); j++){
      if(j != maze->ncol){
        /*if((printed_char = fprintf(outputFILE,"%c",maze->maze_array[i][j])) != 1){
          fprintf(stderr,"Wrong number(%d) of chars printed in visitedfile",printed_char);
          if(printed_char == 0){
            fclose(outputFILE);
            return -1;
          }
        }*/
        fprintf(outputFILE,"%c",maze->maze_array[i][j]);
      }
      else
        fprintf(outputFILE,"\n");
      wordCount++;
    }
  }
  fclose(outputFILE);
  return wordCount;
}

void Find_maze_dimensions(FILE *fptr, int *nrow, int *ncol)
{
   fseek(fptr,0,SEEK_SET);
   *nrow = *ncol = 0;
   char ch;

   while((ch=fgetc(fptr)) != EOF){
    if(ch=='\n')
      (*nrow)++;
    if(*nrow==0)
      (*ncol)++;
   }
  return;
}

int Find_opening_location(FILE *fptr)
{
   fseek(fptr,0,SEEK_SET);
   int col = 0;
   char ch;
   while((ch = fgetc(fptr))!=' '){
    col++;
   }
   return col;
}

nodePath *ln_construct(char ch){
  nodePath * new_node = (nodePath *) malloc(sizeof(nodePath));
  if(new_node == NULL){
    fprintf(stderr,"Failure to create new node in list");
    return NULL;
  }
  
  new_node->nextChar = NULL;
  new_node->ch = ch;
  return new_node;
}


nodePath *ln_insert(nodePath **current_list , char ch){
  nodePath *new_node_to_insert = ln_construct(ch);
  if(new_node_to_insert == NULL){
    fprintf(stderr,"Malloc fail!");
    return NULL;
  }
  
  new_node_to_insert->nextChar = *current_list;
  *current_list = new_node_to_insert;
  return new_node_to_insert;
}

void ln_path_destroy(nodePath *head){
  nodePath *tmp = head->nextChar; 
  while(head != NULL){
    tmp = head->nextChar;
    free(head);
    head = tmp; 
  }
}
//Returns the pointer to the list with a removed char
nodePath *rmv_one_step(nodePath **list){
  if((*list) == NULL){
    fprintf(stderr,"error removing step");
    return NULL;
  }
  
  nodePath *removed_char = *list;
  *list = removed_char->nextChar;
  removed_char->nextChar=NULL;
  ln_path_destroy(removed_char);    
  return *list;
}

nodePath *ln_copy(nodePath *current_list){
  nodePath * next = current_list;
  nodePath * new;
  if(current_list == 0){
    fprintf(stderr,"Error copying linked_list");
    return NULL;
  }
  new = ln_construct(next->ch);
  next = next -> nextChar;
  while(next != NULL){
    new = ln_insert(&new,next->ch);
    next = next->nextChar;
  }
  return new;
}

