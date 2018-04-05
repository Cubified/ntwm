/*
 * dynam.h: dynamic array implementation
 */

#ifndef __DYNAM_H
#define __DYNAM_H

#include <stdlib.h>

typedef struct arr {
  int count;
  Window data[1];
} arr;

static void arr_resize(arr *array, int newsize);
static void arr_push(arr *array, Window win);
static void arr_pop(arr *array, Window win);
static void arr_free(arr *array);
static arr *arr_new(Window win);
static arr *arr_init();

void arr_resize(arr *array, int newsize){
  int oldsize = array->count;
  array->count = newsize;

  Window newdata[newsize];
  for(int i=0;i<oldsize;i++){
    newdata[i] = array->data[i];
  }
  struct arr_new {
    int count;
    Window data[newsize];
  };
  arr *newarr = malloc(sizeof(struct arr_new));
  newarr->count = newsize;
  for(int i=0;i<newsize;i++){
    newarr->data[i] = newdata[i];
  }

  array = newarr;
}

void arr_push(arr *array, Window win){
  arr_resize(array, array->count+1);
  array->data[array->count-1] = win;
}

void arr_pop(arr *array, Window win){
  int index = 0;
  int found = 0;
  for(int i=0;i<array->count;i++){
    if(array->data[i] == win){
      index = i;
      found = 1;
    }
  }
  if(found){
    for(int i=index;i<array->count-1;i++){
      array->data[i] = array->data[i+1];
    }
    arr_resize(array, array->count-1);
  }
}

arr *arr_new(Window win){
  arr *array = malloc(sizeof(arr));
  array->count = 1;
  array->data[0] = win;
  return array;
}

arr *arr_init(){
  arr *array = arr_new(1234);
  arr_pop(array,1234);
  return array;
}

void arr_free(arr *array){
  free(array);
}
#endif
