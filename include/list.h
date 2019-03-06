/*
 * list.h: linked list implementation
 */

#ifndef __LIST_H
#define __LIST_H

typedef struct node {
  void *data;
  unsigned int data_noptr;
  int size;
  struct node *next;
  struct node *prev;
  struct node *end;
} node;

#define list_foreach(root)        node *itr;for(itr=root;itr!=NULL;itr=itr->next)
#define list_foreach_noroot(root) node *itr;for(itr=root->next;itr!=NULL;itr=itr->next)
/*
 * (For ANSI compliance)
 */
#define list_foreach_nodecl(root) for(itr=root;itr!=NULL;itr=itr->next)
#define list_foreach_noroot_nodecl(root) for(itr=root->next;itr!=NULL;itr=itr->next)

#define list_foreach_reverse(root)        node *itr;for(itr=root->end;itr!=NULL;itr=itr->prev)
#define list_foreach_reverse_noroot(root) node *itr;for(itr=root->end;itr!=root;itr=itr->prev)

static node *node_create();

static node *list_init();
static node *list_push(node *root);
static node *list_find(node *root, void *data, unsigned int data_noptr);
static node *list_get(node *root, int index);

static void list_pop(node *root, node *elem);
static void list_free(node *root);

static int list_sizeof(node *root);

/*
 * Creates a new node (not to be used
 * in the majority of cases)
 */
node *node_create(){
  node *elem = malloc(sizeof(node));

  return elem;
}

/*
 * Creates (and returns) a new root node
 */
node *list_init(){
  node *root = node_create();
  root->data = NULL;
  root->data_noptr = 0;
  root->size = 0;
  root->next = NULL;
  root->prev = NULL;
  root->end = NULL;
  
  return root;
}

/*
 * Creates (and returns) a new node which
 * has been inserted after the root node
 */
node *list_push(node *root){
  node *elem = node_create();

  elem->size = 0;
  elem->next = NULL;
  elem->prev = root->end;
  elem->end = NULL;

  if(root->end != NULL){
    root->end->next = elem;
  }
  root->end = elem;

  if(root->next == NULL){
    root->next = elem;
    elem->prev = root;
  }

  root->size++;

  return elem;
}

/*
 * Finds a node based on given data,
 * returns NULL if not found
 */
node *list_find(node *root, void *data, unsigned int data_noptr){
  int found = 0;

  list_foreach(root){
    if((data != NULL &&
        itr->data == data) ||
       (data_noptr != 0 &&
        itr->data_noptr == data_noptr)
      ){
      found = 1;
      break;
    }
  }
  if(found){
    return itr;
  }
  return NULL;
}

/*
 * Gets a node at a specified position
 * in the list (root is at index 0)
 */
node *list_get(node *root, int index){
  int cnt = 0;
  list_foreach(root){
    if(cnt == index){
      break;
    }
    cnt++;
  }
  return itr;
}

/*
 * Deletes a node from the list (does
 * nothing if not found)
 */
void list_pop(node *root, node *elem){
  elem->prev->next = elem->next;
  
  if(elem->next != NULL){
    elem->next->prev = elem->prev;
  }

  if(root->end == elem){
    if(elem->next == NULL){
      root->end = elem->prev;
    } else {
      root->end = elem->next;
    }
  }

  free(elem);

  root->size--;
}

/*
 * Frees an entire list
 */
void list_free(node *root){
  /*
  list_foreach_noroot(root){
    if(itr->prev != NULL){
      free(itr->prev);
    }
  }
  free(itr);*/

  list_foreach_reverse(root){
    if(itr->next != NULL){
      free(itr->next);
    }
  }
}

/*
 * Returns the number of nodes in
 * a list (not including root)
 */
int list_sizeof(node *root){
  return root->size;
}

#endif
