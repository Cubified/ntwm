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
static void screenchange_notify(XEvent *e);

/*
 * Adds a new window to the focused
 * monitor and updates the tiling
 */
void map_request(XEvent *e){
  XMapRequestEvent *ev = &e->xmaprequest;
  int wintype = window_gettype(ev->window);
  
  last_call = "map";
  
  if((wintype == window_normal || wintype == window_utility) && !is_child(ev->window)){
    node *list = find_monitor()->windows;

    if(list_find(list,NULL,ev->window) == NULL){
      node *n = list_push(list);
      n->data_noptr = ev->window;

      tile();

      select_input(ev->window);
      establish_keybinds(ev->window);

      XMapWindow(dpy,ev->window);

      set_focused(ev->window);
    }
  } else {
    /*
     * Window type is not known
     * or not supported, simply
     * display it
     */
    XMapWindow(dpy,ev->window);
  }
  
  if(wintype == window_dock || wintype == window_taskbar){
    multihead_addbar(ev->window);
    tile();
  } else if(wintype == window_dialog){
    select_input(ev->window);
    establish_keybinds(ev->window);
    center_window(ev->window);
    set_focused(ev->window);
  }
}

/*
 * Updates a window's configuration
 */
void configure_request(XEvent *e){
  XConfigureRequestEvent *ev = &e->xconfigurerequest;
  int wintype = window_gettype(ev->window);

  last_call = "configure";
  if(wintype != window_dialog){
    move_resize(
      ev->window,
      ev->x, ev->y,
      ev->width, ev->height
    );
    tile();
  }
}

/*
 * Removes a window from the list
 * of its monitor, updates its
 * tiling
 */
void unmap_notify(XEvent *e){
  XUnmapEvent *ev = &e->xunmap;
  node *list = find_monitor()->windows;
  node *elem = list_find(list,NULL,ev->window);

  last_call = "unmap";

  if(focused == ev->window){
    if(list->size > 1){
      cycle_windows(list,focused,0);
    } else {
      focused = 0;
    }
  }

  if(elem != NULL){
    list_pop(list,elem);
    tile();
  } else {
    node *bar = list_find(bars,NULL,ev->window);
    if(bar != NULL){
      multihead_delbar(bar->data_noptr);
      tile();
    }
  }
}

/*
 * Handles keypresses using keybinds
 * defined in config.h
 */
void key_press(XEvent *e){
  int i;
  XKeyEvent *ev = &e->xkey;
  KeySym keysym = XkbKeycodeToKeysym(dpy, ev->keycode, 0, 0);
  monitor *current_monitor = find_monitor();

  last_call = "keypress";

  for(i=0;i<LENGTH(keys);i++){
    if(keys[i].keysym == keysym &&
      keys[i].mod == ev->state){
      if(strcmp(keys[i].func,"quit") == 0){
        quit();
      } else if(strcmp(keys[i].func,"spawn") == 0){
        spawn(keys[i].arg);
      } else if(strcmp(keys[i].func,"kill") == 0){
        kill_focused();
      } else if(strcmp(keys[i].func,"gaps") == 0){
        toggle_gaps(current_monitor);
      } else if(strcmp(keys[i].func,"full") == 0){
        toggle_fullscreen(current_monitor,focused);
      } else if(strcmp(keys[i].func,"next") == 0){
        cycle_monitors(focused,1);
      } else if(strcmp(keys[i].func,"prev") == 0){
        cycle_monitors(focused,0);
      } else if(strcmp(keys[i].func,"mode") == 0){
        next_mode();
      } else if(strcmp(keys[i].func,"nwin") == 0){
        cycle_windows(current_monitor->windows,focused,1);
      } else if(strcmp(keys[i].func,"pwin") == 0){
        cycle_windows(current_monitor->windows,focused,0);
      } else if(strcmp(keys[i].func,"reset") == 0){
        reset();
      } else {
        error("Unrecognized command \"%s\" in config.",keys[i].func);
      }
    }
  }
}

/*
 * Changes the focused window
 * based on the mouse
 * entering a new
 * window
 */
void enter_notify(XEvent *e){
  XCrossingEvent *ev = &e->xcrossing;
  last_call = "enter";

  set_focused(ev->window);
}

/*
 * Handles Xrandr screenchange event,
 * updating monitor configuration
 * and window tiling
 */
void screenchange_notify(XEvent *e){
  last_call = "screenchange";

  multihead_resize();

  tile();
}

#endif
