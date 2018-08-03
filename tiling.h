/*
 * tiling.h: tiling utilities and modes
 */

#ifndef __TILING_H
#define __TILING_H

static void tile();
static void toggle_gaps(monitor *current_monitor);
static void toggle_fullscreen(monitor *current_monitor, Window win);
static void cycle_monitors(Window win, int dir);
static void next_mode();

/*
 * Iterates over each monitor,
 * runs the set tiling mode
 * per config.h
 */
void tile(){
  monitor *m;
  list_foreach_noroot(monitors){
    m = itr->data;
    
    modes[mode_index](m);
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
void cycle_monitors(Window win, int dir){ // 0 is prev, 1 is next
  if(valid_window(focused)){
    monitor *current_mon = monitors->next->data,
            *next_mon = monitors->next->data;
    node *elem = current_mon->windows->next,
         *n;
#if (dir == 1)
    list_foreach_noroot(monitors){
#elif (dir == 0)
    list_foreach_reverse_noroot(monitors){
#endif
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

      XWarpPointer(
        dpy, 
        None,
        root,
        0, 0,
        0, 0,
        next_mon->x + (next_mon->width / 2),
        next_mon->y + (next_mon->height / 2)
      );
      XFlush(dpy);

      tile();
    }
  }
}

/*
 * Rotates to the next tiling mode
 * (not yet working)
 */
void next_mode(){
  mode_index++;
  if(mode_index >= LENGTH(modes)){
    mode_index = 0;
  }
  tile();
}

#endif
