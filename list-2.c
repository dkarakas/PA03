#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _lnode {
   char *str;
   struct _lnode *next;
} lnode;

lnode *lnode_construct(char *str)
{
   lnode *ret_node = (lnode *)malloc(sizeof(lnode));
   if (ret_node == NULL) {
      fprintf(stderr, "malloc fail!!!!\n");
      return NULL;
   }
   ret_node->str = str;
   ret_node->next = NULL;
   return ret_node;
}

// insert in front of the list

lnode *lnode_insert(lnode **list, char *str)
{
   if (str == NULL) {
      return NULL;
   }
   lnode *new_node = lnode_construct(str);
   if (new_node == NULL) {
      fprintf(stderr, "malloc fail!!!!\n");
      return NULL;
   }
   new_node->next = *list;
   *list = new_node;
   return new_node;
}

// remove the first item in the list

lnode *lnode_remove(lnode **list)
{
   if (*list == NULL) {
      return NULL;
   }
   lnode *ret_node = *list;  // ret_node stores the address to the first item
   *list = ret_node->next;   // *list updated to the address of the 2nd item
   ret_node->next = NULL;    // the removed first item is a single-node list
   return ret_node;
}

void lnode_destroy(lnode *head)
{
   while (head != NULL) {
      // whatever we need from head, we have to gather them first
      // should not access anything in head after we free it
      lnode *tmp = head->next;
      free(head->str);
      free(head);
      head = tmp;
   }
}

int main(int argc, char **argv)
{
   lnode *head = NULL;

   char *new_string = (char *)malloc(sizeof(char)*(strlen("abcd")+1));
   strcpy(new_string, "abcd");
   lnode *new_node = lnode_insert(&head, new_string);

   new_string = (char *)malloc(sizeof(char)*(strlen("efgh")+1));
   strcpy(new_string, "efgh");
   new_node = lnode_insert(&head, new_string);

   new_string = (char *)malloc(sizeof(char)*(strlen("wxyz")+1));
   strcpy(new_string, "wxyz");
   new_node = lnode_insert(&head, new_string);

   new_node = head;
   while (new_node != NULL) {
      printf("%s->", new_node->str);
      new_node = new_node->next;
   }
   printf("NULL\n");

   while (head != NULL) {
      lnode *removed_node = lnode_remove(&head);
      printf("%s\n", removed_node->str);
      lnode_destroy(removed_node); 
   }

   lnode_destroy(head);

   return EXIT_SUCCESS;
}
