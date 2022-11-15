#include <unistd.h>     //ssize_t
#include <stdlib.h>     //malloc(), free()
#include "linked_list.h"
#include "my_c.h"

/* Initialize the linked list to keep the history. */
List *init_LL(int k)
{
  List *p = malloc(sizeof(List));
  p->n_Lines = 0;
  p->max_Lines = k;
  p->head = NULL;
  p->tail = NULL;
  return p;
}

/* Add a history item to the end of the list.
   List* list - the linked list
   char* str - the string to store
   size_t length - the number of bytes use by str */
ssize_t add_item(List *list, char *str, size_t length)
{
  Item *new = malloc(sizeof(Item)); //Make space for the new Item

  if (new == NULL) return -1; //If we couldn't get the space needed from malloc

  if (list->head == NULL) {   //If list is empty
    list->n_Lines = 1;
    list->head = new;
    list->tail = new;
  } else {
    list->tail->next = new;         //Add a line after tail
    list->tail = list->tail->next;  //Move tail one up
    ++list->n_Lines;                //Line was added
    if (list->n_Lines > list->max_Lines) //We pass the limit of line
      move_head(list);  //Move head one up and free space used by previous head
  }

  //Populate the information of the new Item
  new->str = str;
  new->length = length;
  new->next = NULL;
  return 0;
}

/*Move head one up and free space used by previous head*/
void move_head(List *list)
{
  Item *temp = list->head;      //Save previous head

  if (temp == NULL)             //List is empty
    return;
  else if (temp == list->tail)  //We would eliminate last element in list
    list->tail = temp->next;

  list->head = list->head->next;//Move head one up
  free_item(temp);              //Free space used by previous head
  --list->n_Lines;              //Line was removed
  return;
}

/*Print the entire content of the list. */
ssize_t print_lines(List *list)
{
  Item *curr = list->head;
  while (curr != NULL) {
    if (my_write(1, curr->str, curr->length) < 0)
      return -1;
    curr = curr->next;
  }
  return 0;
}

/*Free item space use in malloc. */
void free_item(Item *item)
{
  free(item->str);
  free(item);
}

/*Free history list. */
void free_history(List *list)
{
  if (list->head == NULL) {
    free(list);
    return;
  }
  Item *curr = list->head;
  Item *next;           //Dummy variable to hold Line after curr
  while (curr->next != NULL) {
    next = curr->next;  //Save next item so we can free curr space without losing next item
    free_item(curr);
    curr = next;
  }
  //Free the space used for the last Line
  free_item(curr);
  free(list);
  return;
}

