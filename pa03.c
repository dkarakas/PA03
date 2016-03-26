#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include "answer03.h"

// declaration of a function defined answer03.c
int Find_path_from_top_entrance_to_bottom_exit(char *mazefile, char *directionfile, char *visitedfile);

int main(int argc, char* argv[]) 
{
   // the first argument is the option "-s" or "-m"
   // -s means to perform simulation 
   // in that case, the second argument is the maze file and 
   // the third argument is the direction file, both files should be read
   // the fourth argument is the result of the simulation, with the visited 
   // locations marked as '.'  You should show the movement until the first 
   // failed location
   // the fifth argument through eighth argument specify the row and column
   // coordinates of the source and the row and column coordinates of the
   // destination
   //
   // -m means to determine the moves
   // in that case, the second argument is the maze file, which is read and
   // the third argument is the directions file, which is to be written
   // the fourth argument through seventh argument specify the row and column
   // coordinates of the source and the row and column coordinates of the
   // destination
   // 
   // -t means to find a path from top-most, left-most entrance to 
   // the bottom-most, right-most exit
   // in that case, the second argument is the maze file, which is read and
   // the third argument is the direction file, which is to be written
   // the fourth argument stores the explored locations in the maze while
   // looking for this path
   //
   
   if (argc < 2) 
   {
       return EXIT_FAILURE;
   }

  
   if (strcmp("-t", argv[1]) == 0) {
      if (argc != 5) { 
         return EXIT_FAILURE; 
      }
      int step_count = Find_path_from_top_entrance_to_bottom_exit(argv[2], argv[3]
									, argv[4]);
      printf("%d\n", step_count); 

   } else if(strcmp("-s", argv[1]) == 0){ 
     if (argc != 9)
        return EXIT_FAILURE; 
     Coord source;
     Coord destination;
     errno = 0;
     char * endptr; 
     source.row = strtol(argv[5],&endptr,10);
     if(*endptr != '\0'|| (errno != 0 && source.row ==0) ){
       fprintf(stderr,"Invalid charaters %s",endptr);
       return EXIT_FAILURE;
     }
     source.col = strtol(argv[6],&endptr,10);
     if(*endptr != '\0' || (errno != 0 && source.col ==0) ){
       fprintf(stderr,"Invalid charaters %s",endptr);
       return EXIT_FAILURE;
     }
     destination.row= strtol(argv[7],&endptr,10);
     if(*endptr != '\0'|| (errno != 0 && destination.row ==0) ){
       fprintf(stderr,"Invalid charaters %s",endptr);
       return EXIT_FAILURE;
     }
     destination.col = strtol(argv[8],&endptr,10);
     if(*endptr != '\0'|| (errno != 0 && destination.col ==0) ){
       fprintf(stderr,"Invalid charaters %s",endptr);
       return EXIT_FAILURE;
     }
 
     int step_count = Simulate_movement(argv[2],argv[3], argv[4],
                                        source, destination);
     printf("%d\n", step_count); 
   } else if(strcmp("-m", argv[2]) ==0){}

    else{
      return EXIT_FAILURE;
   }

  return EXIT_SUCCESS;    
}
