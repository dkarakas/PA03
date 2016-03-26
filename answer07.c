#include <stdio.h>
#include <stdlib.h>
#include "answer07.h"

/* Determine the dimensions of the maze contain in file. */
/* The maze will always be of the correct format */
/* the number of rows is to be stored at location pointed to by nrow */ 
/* the number of ncol is to be stored at location pointed to by ncol */

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

/* Determine the oolumn location of the top opening */
/* you may assume that the maze is of the correct format */

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

/* Count the number of locations that are PATH */
/* A location is PATH if it is a space, ' ', or PATH */
/* you may assume that the maze is of the correct format */

int Count_path_locations(FILE *fptr)
{
   fseek(fptr,0,SEEK_SET);
   int path = 0;
   char ch;
   while((ch = fgetc(fptr))!=EOF)
     if(ch==' ')
       path++;
   return path;
}

/* Return the type of location: WALL or PATH */
/* The location is specified by its row-column coordinates. */
/* The coordinates are specified by row and col */
/* you may assume that these coordinates are always valid */
/* you may assume that the maze is of the correct format */

char Get_location_type(FILE *fptr, int row, int col) {
   fseek(fptr,0,SEEK_SET);
   char ch;
   char char_ret;
   int cur_row =0;
   if(row == 0){
     fseek(fptr,col,SEEK_CUR);
     return fgetc(fptr);
   }
   while((ch = fgetc(fptr))!=EOF){
    if(ch=='\n'){
      cur_row++;
    }
    if(cur_row == row){
      fseek(fptr,col,SEEK_CUR);
      char_ret = fgetc(fptr);
      fseek(fptr,0,SEEK_END);
    }
   }
   return char_ret;
}
 
/* Given a filename, re-represent the maze in the file pointer fptr */
/* in a single line */
/* The return value should be the number of characters written into the */
/* the new file */
/* if the writing to a file fails, you should return -1 */ 
/* you may assume that the maze is of the correct format */

int Represent_maze_in_one_line(char *filename, FILE *fptr)
{
   fseek(fptr,0,SEEK_SET);
   FILE* output = fopen(filename,"w+");
   fseek(output,0,SEEK_SET);
   int ch;
   int count=0;
   while((ch=fgetc(fptr))!=EOF){
    if(ch!='\n'){
      fprintf(output,"%c",ch);
      count++;
    }
   }
  fclose(output); 
  return count;
}
