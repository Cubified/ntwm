/*
 * list.h: linked list implementation
 */

#ifndef __LIST_H
#define __LIST_H

typedef struct node {
  void *data;
  unsigned int data_noptr;
  struct node *next;
  struct node *prev;
  struct node *end;
} node;

//

#define list_foreach(root)        node *itr;for(itr=root;itr!=NULL;itr=itr->next)
#define list_foreach_noroot(root) node *itr;for(itr=root->next;itr!=NULL;itr=itr->next)

//

static node *node_create();

static node *list_init();
static node *list_push(node *root);
static node *list_find(node *root, void *data, unsigned int data_noptr);
static node *list_get(node *root, int index);

static void list_pop(node *root, node *elem);
static void list_free(node *root);

static int list_sizeof(node *root);

//

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

  elem->next = NULL;
  elem->prev = root->end;
  elem->end = NULL;

  if(root->end != NULL){
    root->end->next = elem;
  }
  root->end = elem;

  if(root->next == NULL){
    root->next = elem;
  }

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
  list_foreach(root){
    if(itr->next == elem){
      itr->next = elem->next;
      if(itr->next != NULL){
        itr->next->prev = itr;
      }
      
      free(elem);

      break;
    }
  }
}

/*
 * Frees an entire list
 */
void list_free(node *root){
  list_foreach(root){
    if(itr->prev != NULL){
      free(itr->prev);
    }
  }
}

/*
 * Returns the number of nodes in
 * a list (not including root)
 */
int list_sizeof(node *root){
  int size = -1;
  list_foreach(root){
    size++;
  }
  return size;
}

#endif
