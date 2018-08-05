/*
 * tiling.h: tiling utilities and modes
 */

#ifndef __TILING_H
#define __TILING_H

static void tile();
static void toggle_gaps(monitor *current_monitor);
static void toggle_fullscreen(monitor *current_monitor, Window win);
static void cycle_monitors(Window win, int dir);
static void cycle_windows(node *windows, Window current, int dir);
static void next_mode();

/*
 * Iterates over each monitor,
 * arranges windows using the
 * current tiling mode
 */
void tile(){
  monitor *m;
  list_foreach_noroot(monitors){
    m = itr->data;
    
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
 * Moves a window to either the previous
 * or next available monitor
 */
void cycle_monitors(Window win, int dir){ // 0 is prev, 1 is next (TODO)
  if(valid_window(focused)){
    monitor *current_mon = monitors->next->data,
            *next_mon = monitors->next->data;
    node *elem = current_mon->windows->next,
         *n;
    list_foreach_noroot(monitors){
      current_mon = itr->data;
      elem = list_find(current_mon->windows,NULL,win);
      if(elem != NULL && elem->data_noptr == win){
        if(itr->next != NULL){
          next_mon = itr->next->data;
        } else {
          next_mon = monitors->next->data;
        }
        break;
      }
    }
    if(current_mon != NULL &&
       next_mon != NULL &&
       elem != NULL){
      list_pop(current_mon->windows,elem);
      n = list_push(next_mon->windows);
      n->data_noptr = win;

      set_cursorpos(
        next_mon->x + (next_mon->width / 2),
        next_mon->y + (next_mon->height / 2)
      );

      tile();
    }
  }
}

/*
 * Focuses the previous/next window
 * in a monitor's list
 */
void cycle_windows(node *windows, Window current, int dir){
  node *elem;
  list_foreach_noroot(windows){
    if(itr->data_noptr == current){
      elem = itr;
      break;
    }
  }

  switch(dir){
    case 0:
      if(elem->prev == windows){
        set_focused(windows->end->data_noptr);
      } else {
        set_focused(elem->prev->data_noptr);
      }
      break;
    case 1:
      if(elem->next == NULL){
        set_focused(windows->next->data_noptr);
      } else {
        set_focused(elem->next->data_noptr);
      }
      break;
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

#endif
