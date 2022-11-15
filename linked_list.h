#ifndef _LINKED_LIST_
#define _LINKED_LIST_

typedef struct s_Item {
  char *str;
  size_t length;
  struct s_Item *next;
} Item;

typedef struct s_List {
  int n_Lines;
  int max_Lines;
  struct s_Item *head;
  struct s_Item *tail;
} List;

/* Initialize the linked list to keep the history. */
List *init_LL(int k);

/* Add a history item to the end of the list.
   List* list - the linked list
   char* str - the string to store */
ssize_t add_item(List *list, char *str, size_t length);

/*Move head one up and free space used by previous head*/
void move_head(List *list);

/*Print the entire contents of the list. */
ssize_t print_lines(List *list);

/*Free item space use in malloc. */
void free_item(Item *item);

/*Free history list. */
void free_history(List *list);

#endif
