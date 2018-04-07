/*
 * events.h: Xlib event handlers
 */

#ifndef __EVENTS_H
#define __EVENTS_H

static void map_request(XEvent *e);
static void configure_request(XEvent *e);
static void unmap_notify(XEvent *e);
static void key_press(XEvent *e);
static void enter_notify(XEvent *e);

void map_request(XEvent *e){
  XMapRequestEvent *ev = &e->xmaprequest;

  list_node *list = find_monitor()->windows;

  last_call = "map";

  list_push(list,ev->window);

  tile(list);

  select_input(ev->window);
  establish_keybinds(ev->window);

  XMapWindow(dpy,ev->window);
}

void configure_request(XEvent *e){
  XConfigureRequestEvent *ev = &e->xconfigurerequest;
  last_call = "configure";
  move_resize(
    ev->window,
    ev->x, ev->y,
    ev->width, ev->height
  );
}

void unmap_notify(XEvent *e){
  XUnmapEvent *ev = &e->xunmap;

  last_call = "unmap";

  list_node *list = find_monitor()->windows;

  list_pop(list,ev->window);

  tile(list);
}

void key_press(XEvent *e){
  XKeyEvent *ev = &e->xkey;
  KeySym keysym = XkbKeycodeToKeysym(dpy, ev->keycode, 0, 0);

  last_call = "keypress";

  monitor *current_monitor = find_monitor();
  list_node *list = current_monitor->windows;

  for(int i=0;i<LENGTH(keys);i++){
    if(keys[i].keysym == keysym &&
      keys[i].mod == ev->state){
      if(strcmp(keys[i].func,"quit") == 0){
        quit();
      } else if(strcmp(keys[i].func,"spawn") == 0){
        spawn(keys[i].arg);
      } else if(strcmp(keys[i].func,"kill") == 0){
        kill_focused();
      } else if(strcmp(keys[i].func,"gaps") == 0){
        current_monitor->gaps_enabled = !current_monitor->gaps_enabled;
        tile(list);
      } else if(strcmp(keys[i].func,"full") == 0){
        move_resize(
          focused,
          0, 0,
          current_monitor->width,
          current_monitor->height
        );
      }
    }
  }
}

void enter_notify(XEvent *e){
  XCrossingEvent *ev = &e->xcrossing;
  focused = ev->window;
  last_call = "enter";

  XSetInputFocus(dpy,ev->window,RevertToParent,CurrentTime);
}
#endif
