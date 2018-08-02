/*
 * tiling.h: tiling utilities and modes
 */

#ifndef __TILING_H
#define __TILING_H

static void tile();
static void tile_grid(monitor *m);
static void tile_stack(monitor *m);
static void toggle_gaps(monitor *current_monitor);
static void toggle_fullscreen(monitor *current_monitor, Window win);
static void cycle_monitors(Window win, int dir);
static void next_mode();

void tile(){
  monitor *m;
  list_foreach_noroot(monitors){
    m = itr->data;

    TILING_MODE(m);
  }
}

void tile_grid(monitor *m){
  int clients_count, i,
    cols, rows,
    col_height, col_width,
    col_number, row_number,
    x, y,
    w, h,
    gaps;
  Window win;
  node *list = m->windows;

  clients_count = list_sizeof(list);
  if(clients_count == 0){
    return;
  }

  for(cols = 0; cols <= clients_count / 2; cols++){
    if(cols*cols >= clients_count){
      break;
    }
  }

  rows = clients_count / cols;

  gaps = (m->gaps_enabled ? GAPS : 0);

  col_height = m->height - gaps;
  col_width = (m->width - gaps) / (cols ? cols : 1);
  col_number = 0;
  row_number = 0;
  i = 0;

  list_foreach_noroot(list){
    if(i / rows + 1 > cols - clients_count % cols){
      rows = clients_count / cols + 1;
    }

    x = (col_number * col_width) + gaps + m->x;
    y = row_number * col_height / rows + gaps + m->y;
    w = col_width - gaps;
    h = (col_height / rows) - gaps;

    win = itr->data_noptr;

    if(m->fullscreen_enabled && m->fullscreen == win){
      x = m->x;
      y = m->y;
      w = m->width;
      h = m->height;

      XRaiseWindow(dpy,win);
    }

    move_resize(
      win,
      x, y,
      w, h
    );

    if(++row_number >= rows){
      row_number = 0;
      col_number++;
    }
    i++;
  }
}

// TODO:
// - Cleanup, this is simply awful to look at
// - Allow for tiling mode to be changed without recompilation
void tile_stack(monitor *m){
  node *list = m->windows;
  int len = list_sizeof(list);
  float len_f = (float)len;

  if(len == 0){
    return;
  }

  int gaps = (m->gaps_enabled ? GAPS : 0);
  int cols = (len >= 3 ? 3 : len);

  float subcol_width_exact = m->width * GRID_RATIO;
  int subcol_width = round_float(subcol_width_exact);

  float subcol_height_right_exact = 
    (len > 1 ?                              // Return zero if right column does not need to exist
      (float)m->height /                    // Otherwise, window height is monitor height divided
        ((float)closest_even(len) / 2.0) :  //   by half of the closest even number to the number
      0.0                                   //   of windows (essentially a complex way of writing
    );                                      //   the number of windows in the right column)
  int subcol_height_right = round_float(subcol_height_right_exact);

  float subcol_height_left_exact =
    (len > 2 ?                            // Return zero if the left column does not need to exist
     (float)m->height /                   // Otherwise, window height is monitor height divided by
      (((float)closest_even(len) - 2.0) / //   the number of windows in the left column, see above
        2.0) :                            // A note: every even number of windows corresponds to a
      0.0                                 //   new window in the right column, whereas every odd #
    );                                    //   of windows (apart from 1) corresponds to a new left
                                          //   window
  int subcol_height_left = round_float(subcol_height_left_exact);

  int x = 0, y = 0,
      width = 0, height = 0;

  int i = 0,
      current_col = 0, // Zero is center, 1 is right, 2 is left
      current_row_right = 0,
      current_row_left = 0;

  Window win;

  list_foreach_noroot(list){
    win = itr->data_noptr;

    switch(current_col){
      case 0: // Main, central window
        y = m->y + gaps;
        height = m->height - (gaps * 2);
        switch(cols){
          case 1:
            x = m->x + gaps;
            width = m->width - (gaps * 2);
            break;
          case 2:
            x = m->x + gaps;
            width = (m->width - subcol_width) - (gaps * 2);
            break;
          case 3:
            x = m->x + subcol_width + gaps;
            width = (m->width - (subcol_width * 2)) - (gaps * 2);
            break;
        }
        break;
      case 1: // Right column
        x = m->x + (m->width - subcol_width);
        y = m->y + (subcol_height_right * (current_row_right)) + gaps;
        width = subcol_width - gaps;
        height = subcol_height_right - (gaps * 2);
        current_row_right++;
        break;
      case 2: // Left column
        x = m->x + gaps;
        y = m->y + (subcol_height_left * (current_row_left)) + gaps;
        width = subcol_width - gaps;
        height = subcol_height_left - (gaps * 2);
        current_row_left++;
        break;
    }
    
    if(m->fullscreen_enabled && m->fullscreen == win){
      x = m->x;
      y = m->y;
      width = m->width;
      height = m->height;

      XRaiseWindow(dpy,win);
    }

    move_resize(
      win,
      x, y,
      width, height
    );

    i++;
    current_col++;
    if(current_col >= cols){
      current_col = 1;
    }
  }
}

void toggle_gaps(monitor *current_monitor){
  current_monitor->gaps_enabled = !current_monitor->gaps_enabled;
  tile(current_monitor->windows);
}

void toggle_fullscreen(monitor *current_monitor, Window win){
  current_monitor->fullscreen_enabled = !current_monitor->fullscreen_enabled;
  if(current_monitor->fullscreen_enabled){
    current_monitor->fullscreen = focused;
  }
  tile(current_monitor->windows);
}

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

void next_mode(){
  // TODO
}

#endif
