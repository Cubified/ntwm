/*
 * tiling.h: tiling utilities
 */

#ifndef __TILING_H
#define __TILING_H

static void tile();
static void toggle_gaps(monitor *current_monitor);
static void toggle_fullscreen(monitor *current_monitor, Window win);
static void toggle_above(Window win);
static void cycle_monitors(Window win, int dir);
static void cycle_windows(pool *windows, Window current, int dir);
static void next_mode();
static void center_window(Window win);
static void reset(func map_req);
static void tile_existing(func map_req);

/*
 * Iterates over each monitor,
 * arranges windows using the
 * current tiling mode
 */
void tile(){
  monitor *m;
  pool_foreach(monitors){
    m = pool_get(ind, monitors);

    if(above_enabled && m->fullscreen_enabled){
      m->fullscreen_enabled = 2;
    } else if(!above_enabled && m->fullscreen_enabled == 2){
      m->fullscreen_enabled = 1;
    }
    
    modes[m->mode](m);
  }
}

/*
 * Toggles gaps on a designated monitor
 */
void toggle_gaps(monitor *current_monitor){
  current_monitor->gaps_enabled = !current_monitor->gaps_enabled;
  tile();
}

/*
 * Toggles fullscreen on a designated monitor
 */
void toggle_fullscreen(monitor *current_monitor, Window win){
  current_monitor->fullscreen_enabled = !current_monitor->fullscreen_enabled;
  if(current_monitor->fullscreen_enabled){
    current_monitor->fullscreen = focused;
  }
  tile();
}

/*
 * Toggles a window as above all others,
 * taking precedence over fullscreen as
 * this is a mode explicitly requested
 * by client windows (as per EWMH)
 */
void toggle_above(Window win){
  above_enabled = !above_enabled;
  if(above_enabled){
    above = win;
  }
}

/*
 * Moves a window to either the previous
 * or next available monitor
 */
void cycle_monitors(Window win, int dir){
  if(x_valid_window(win)){
    int v = -1,
        p = 0;
    monitor *m = pool_get(0, monitors);
    pool_foreach(monitors){
      m = pool_get(ind, monitors);
      if((p = pool_find((void*)win, m->windows)) > -1){
        break;
      }
    }

    v = pool_adj(p, dir, monitors);
    if(v != -1){
      pool_pop(p, m->windows);

      m = pool_get(v, monitors);
      pool_push((void*)win, m->windows);

      x_set_cursorpos(
        m->x + (m->width / 2),
        m->y + (m->height / 2)
      );

      tile();
    }
  }
}

/*
 * Focuses the previous/next window
 * in a monitor's list
 */
void cycle_windows(pool *windows, Window current, int dir){
  int v = -1,
      ind = pool_find((void*)current, windows);
  if(ind != -1){
    v = pool_adj(ind, dir, windows);
    if(v != -1){
      x_set_focused((Window)pool_get(v, windows));
    }
  }
}

/*
 * Rotates to the next tiling mode
 */
void next_mode(){
  monitor *m = find_monitor();
  m->mode++;
  if(m->mode >= LENGTH(modes)){
    m->mode = 0;
  }
  tile();
}

/*
 * Centers a window on the
 * current monitor
 */
void center_window(Window win){
  monitor *m = find_monitor();
  int gaps = (m->gaps_enabled ? GAPS : 0);
  x_move_resize(
    win,
    (m->x + gaps) + ((m->width  - (m->width  * DIALOG_RATIO)) / 2),
    (m->y + gaps) + ((m->height - (m->height * DIALOG_RATIO)) / 2),
    (m->width  - (2 * gaps)) * DIALOG_RATIO,
    (m->height - (2 * gaps)) * DIALOG_RATIO
  );
}

/*
 * Resets tiling by clearing
 * all lists, used to remove
 * a "ghost window"
 */
void reset(func map_req){
  pool_foreach(monitors){
    monitor *m = pool_get(ind, monitors);
    if(m != NULL && m->windows != NULL){
      pool_free(m->windows);
    }
    m->windows = pool_init(MAX_WINDOWS);
  }

  /*
   * map_request() has not
   * yet been declared by
   * this point - is such
   * a practice a hack or
   * bad habit?
   */
  tile_existing(map_req);
}

/*
 * Queries the root window
 * for existing windows,
 * managing them
 * accordingly
 */
void tile_existing(func map_req){
  XConfigureRequestEvent evt;
  XWindowAttributes attr;
  Window root,
         parent;
  Window *children;
  unsigned int children_count;
  int n;

  XQueryTree(
    dpy,
    DefaultRootWindow(dpy),
    &root,
    &parent,
    &children,
    &children_count
  );

  for(n=0;n<children_count;n++){
    evt.display = dpy;
    evt.window = children[n];

    XGetWindowAttributes(dpy, children[n], &attr);

    if(attr.map_state == IsViewable &&
       !attr.override_redirect &&
       !multihead_isbar(children[n])){
      map_req((XEvent*)&evt);
    }
  }

  XFree(children);
}

#endif
