/*
 * multihead.h: multimonitor support
 */

#ifndef __MULTIHEAD_H
#define __MULTIHEAD_H

typedef struct monitor {
  list_node *windows;
  int width;
  int height;
  int x;
  int y;
} monitor;

typedef struct node {
  monitor *monitor;
  struct node *next;
  struct node *prev;
} node;

static void node_free(node *node);
static void node_push(node *root, monitor *monitor);
static void node_pop(node *root, monitor *monitor);
static node *node_get(node *root, int index);
static node *node_init();

static void multihead_setup();
static void multihead_free();
static monitor *find_monitor();

node *node_init(){
  node *new_node = malloc(sizeof(node));
  new_node->monitor = NULL;
  new_node->next = NULL;
  new_node->prev = NULL;
  return new_node;
}

void node_free(node *node){
  free(node);
}

void node_push(node *root, monitor *monitor){
  node *iterator = root;
  while(iterator->next != NULL){
    iterator = iterator->next;
  }
  node *new_node = malloc(sizeof(node));
  new_node->monitor = monitor;
  new_node->next = NULL;
  new_node->prev = iterator;
  iterator->next = new_node;
}

void node_pop(node *root, monitor *monitor){
  node *iterator = root;
  while(iterator->next->monitor != monitor){
    iterator = iterator->next;
  }
  iterator->next = iterator->next->next;
  iterator->next->prev = iterator;
}

node *node_get(node *root, int index){
  node *iterator = root;
  for(int i=0;i<index+1;i++){
    iterator = iterator->next;
  }
  return iterator;
}

node *monitors;

#ifdef MULTIHEAD

#include <xcb/xcb.h>
#include <xcb/randr.h>

void multihead_setup(){
  monitors = node_init();

  int crtc_count = 0;
  xcb_connection_t *conn;
  xcb_screen_t *scr;
  xcb_window_t dummy;
  xcb_randr_get_screen_resources_cookie_t cookie;
  xcb_randr_get_screen_resources_reply_t *reply;
  xcb_randr_crtc_t *first;

  conn = xcb_connect(0,0);
  scr = xcb_setup_roots_iterator(xcb_get_setup(conn)).data;
  dummy = xcb_generate_id(conn);
  xcb_create_window(conn,0,dummy,scr->root,0,0,1,1,0,0,0,0,0);
  xcb_flush(conn);
  cookie = xcb_randr_get_screen_resources(conn,dummy);
  reply = xcb_randr_get_screen_resources_reply(conn,cookie,0);
  crtc_count = xcb_randr_get_screen_resources_crtcs_length(reply);
  first = xcb_randr_get_screen_resources_crtcs(reply);
  xcb_randr_get_crtc_info_cookie_t res_cookie[crtc_count];

  for(int i=0;i<crtc_count;i++){
    res_cookie[i] = xcb_randr_get_crtc_info(conn,*(first+i),0);
  }
    
  xcb_randr_get_crtc_info_reply_t *res_reply[crtc_count];

  for(int i=0;i<crtc_count;i++){
    res_reply[i] = xcb_randr_get_crtc_info_reply(conn,res_cookie[i],0);

    monitor *monitor = malloc(sizeof(monitor));
    monitor->windows = list_init();
    monitor->width = res_reply[i]->width;
    monitor->height = res_reply[i]->height;
    monitor->x = res_reply[i]->x;
    monitor->y = res_reply[i]->y;
    node_push(monitors,monitor);
  }

  xcb_disconnect(conn);
}

void multihead_free(){
  node *iterator = monitors;
  while(iterator != NULL){
    if(iterator->monitor != NULL){
      list_freeall(iterator->monitor->windows);
    }
    iterator = iterator->next;
    if(iterator != NULL && iterator->prev != NULL){
      node_free(iterator->prev);
    }
  }
}

monitor *find_monitor(){
  Window win;
  int pos_x, pos_y,
    win_x, win_y;
  unsigned int mask_return;
  XQueryPointer(dpy,DefaultRootWindow(dpy),&win,&win,&pos_x,&pos_y,&win_x,&win_y,&mask_return);

  node *iterator = monitors;
  int found = 0;
  while(iterator != NULL && !found){
    if(iterator->monitor != NULL){
      if(pos_x <= iterator->monitor->x+iterator->monitor->width &&
        pos_x >= iterator->monitor->x &&
        pos_y <= iterator->monitor->y+iterator->monitor->height &&
        pos_y >= iterator->monitor->y){
        found = 1;
      } else {
        iterator = iterator->next;
      }
    } else {
      iterator = iterator->next;
    }
  }
  if(found){
    return iterator->monitor;
  }
  return NULL;
}

#endif

#ifndef MULTIHEAD

void multihead_setup(){
  monitors = node_init();
  monitor *monitor = malloc(sizeof(monitor));
  monitor->windows = arr_init();
  monitor->width = XWidthOfScreen(screen);
  monitor->height = XHeightOfScreen(screen);
  monitor->x = 0;
  monitor->y = 0;
  node_push(monitors,monitor);
}

void multihead_free(){
  node *iterator = monitors;
  while(iterator != NULL){
    if(iterator->monitor != NULL){
      arr_free(iterator->monitor->windows);
    }
    iterator = iterator->next;
    if(iterator != NULL && iterator->prev != NULL){
      node_free(iterator->prev);
    }
  }
}

monitor *find_monitor(){
  return node_get(monitors,0)->monitor;
}

#endif

#endif
