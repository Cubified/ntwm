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
static void client_message(XEvent *e);
static void screenchange_notify(XEvent *e);

/*
 * Adds a new window to the focused
 * monitor and updates the tiling
 */
void map_request(XEvent *e){
  XMapRequestEvent *ev = &e->xmaprequest;
  int wintype = x_window_gettype(ev->window);
  
  last_call = "map";
  
  if((wintype == window_normal ||
      wintype == window_utility) &&
      !x_is_child(ev->window)){
    pool *list = find_monitor()->windows;

    if(pool_find((void*)ev->window, list) == -1){
      pool_push((void*)ev->window, list);

      tile();

      x_select_input(ev->window);
      x_establish_keybinds(ev->window);

      XMapWindow(dpy, ev->window);

      x_set_focused(ev->window);
    }
  } else {
    /*
     * Window type is not known
     * or not supported, simply
     * display it
     */
    XMapWindow(dpy, ev->window);
  }

  /*
   * Window with _NET_WM_STATE_ABOVE
   * set always has precedence
   */
  if(above_enabled &&
     above != 0){
    XMapRaised(dpy, above);
  }
  
  if(wintype == window_dock || wintype == window_taskbar){
    multihead_addbar(ev->window);
    tile();
  } else if(wintype == window_dialog){
    x_select_input(ev->window);
    x_establish_keybinds(ev->window);
    center_window(ev->window);
    x_set_focused(ev->window);
  }
}

/*
 * Updates a window's configuration
 */
void configure_request(XEvent *e){
  XConfigureRequestEvent *ev = &e->xconfigurerequest;
  int wintype = x_window_gettype(ev->window);

  last_call = "configure";
  if(wintype != window_dialog){
    x_move_resize(
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
  pool *list = find_monitor()->windows;
  int ind = pool_find((void*)ev->window, list);

  last_call = "unmap";

  if(focused == ev->window){
    if((unsigned int)~list->avail > 0){
      cycle_windows(list, focused, 0);
    } else {
      focused = 0;
    }
  }
  
  if(above == ev->window){
    above_enabled = 0;
    above = 0;
  }

  if(ind > -1){
    pool_pop(ind, list);
  } else {
    int p;
    if((p = pool_find((void*)ev->window, bars)) > -1){
      multihead_delbar(p);
    }
  }

  tile();
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
      switch(keys[i].func){
        case cmd_quit:
          quit();
          break;
        case cmd_spawn:
          spawn(keys[i].arg);
          break;
        case cmd_kill:
          x_kill_focused();
          break;
        case cmd_gaps:
          toggle_gaps(current_monitor);
          break;
        case cmd_full:
          toggle_fullscreen(current_monitor, focused);
          break;
        case cmd_next:
          cycle_monitors(focused, DIR_RIGHT);
          break;
        case cmd_prev:
          cycle_monitors(focused, DIR_LEFT);
          break;
        case cmd_mode:
          next_mode();
          break;
        case cmd_nwin:
          cycle_windows(current_monitor->windows, focused, 1);
          break;
        case cmd_pwin:
          cycle_windows(current_monitor->windows, focused, 0);
          break;
        case cmd_reset:
          reset(map_request);
          break;
        default:
#ifdef LOGGING_NO_STDIO
          error("Unrecognized command in config.");
#else
          error("Unrecognized command \"%s\" in config.", keys[i].func);
#endif
          break;
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

  x_set_focused(ev->window);
}

/*
 * Handles a specific subset of
 * client message events
 * (incomplete for now)
 */
void client_message(XEvent *e){
  XClientMessageEvent *ev = &e->xclient;

  if(ev->message_type == atoms[atom_state]){
    if(ev->data.l[1] == atoms[atom_above] ||
       ev->data.l[2] == atoms[atom_above]){
      x_set_property(atoms[atom_above], XA_WINDOW, 32, (unsigned char*)&ev->window);
      toggle_above(ev->window);
    } else if(ev->data.l[1] == atoms[atom_fullscreen] ||
              ev->data.l[2] == atoms[atom_fullscreen]){
      int x, y;
      unsigned int w, h;
      x_set_property(atoms[atom_fullscreen], XA_WINDOW, 32, (unsigned char*)&ev->window);
      x_query_window(ev->window, &x, &y, &w, &h);
      toggle_fullscreen(monitor_atpos(x, y), ev->window);
    }
  }
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
