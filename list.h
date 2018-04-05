/*
 * list.h: singly-linked list
 */

#ifndef __LIST_H
#define __LIST_H

#include <stdlib.h>

typedef struct list_node {
  Window window;
  struct list_node *next;
} list_node;

#define LIST_NODE_FOREACH(n)        list_node *iterator;for(iterator=n;iterator!=NULL;iterator=iterator->next)
#define LIST_NODE_FOREACH_NOROOT(n) list_node *iterator;for(iterator=n->next;iterator!=NULL;iterator=iterator->next)

static void list_push(list_node *root, Window win);
static void list_pop(list_node *root, Window win);
static void list_free(list_node *n);
static void list_freeall(list_node *root);
static int list_sizeof(list_node *root);
static list_node *list_init();

void list_push(list_node *root, Window win){
  list_node *n = list_init();
  n->window = win;
  n->next = root->next;
  root->next = n;
}

void list_pop(list_node *root, Window win){
  LIST_NODE_FOREACH(root){
    if(iterator->next != NULL && iterator->next->window == win){
      break;
    }
  }
  list_node *copy = iterator->next->next;
  list_free(iterator->next);
  iterator->next = copy;
}

void list_free(list_node *n){
  free(n);
}

void list_freeall(list_node *root){
  LIST_NODE_FOREACH(root){
    list_node *copy = iterator;
    free(copy);
  }
}

int list_sizeof(list_node *root){
  int size = -1;
  LIST_NODE_FOREACH(root){size++;}
  return size;
}

list_node *list_init(){
  list_node *new_node = malloc(sizeof(list_node));
  new_node->window = (Window)NULL;
  new_node->next = NULL;
  return new_node;
}

#endif
