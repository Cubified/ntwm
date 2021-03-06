/*
 * modes.h: tiling modes
 */

#ifndef __MODES_H
#define __MODES_H

/*
 * Function definitions are in config.h,
 * and take advantage of the fact that
 *
 *    static void function()
 *
 * is not the same as
 *
 *    static void function(void)
 *
 * meaning it is possible to declare
 * additional parameters (such as a
 * pointer to a monitor) later if
 * (void) is not specified.
 */

/*
 * This is relevant because the monitor
 * struct does not exist in config.h
 * (because multihead.h is included
 * after), but does in modes.h.
 */

/*
 * The purpose of this is to (somewhat)
 * cleanly separate tiling logic from
 * its customization without keeping
 * all code in ntwm.c (dwm being an
 * example of this).
 */

#ifdef ENABLE_GRID

/*
 *  ___________
 * |  M  |  2  |
 * |_____|_____|
 * |  1  |  3  |
 * |_____|_____|
 */
void tile_grid(monitor *m){
  int clients_count, i,
    cols, rows,
    col_height, col_width,
    col_number, row_number,
    x, y,
    w, h,
    gaps,
    ind;
  Window win;
  pool *list = m->windows;
  
  clients_count = pool_count(list);
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

  pool_foreach_nodecl(list){
    if(i / rows + 1 > cols - clients_count % cols){
      rows = clients_count / cols + 1;
    }

    x = (col_number * col_width) + gaps + m->x;
    y = row_number * col_height / rows + gaps + m->y;
    w = col_width - gaps;
    h = (col_height / rows) - gaps;

    win = (Window)pool_get(ind, list);

    if(m->fullscreen_enabled &&
       m->fullscreen == win &&
       !above_enabled){
      x = m->x;
      y = m->y;
      w = m->width;
      h = m->height;

      XRaiseWindow(dpy, win);
    }

    x_move_resize(
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

#endif

#ifdef ENABLE_DUALSTACK

/*
 *  ___________
 * | 2|     | 1|
 * |__|  M  |__|
 * | 4|     | 3|
 * |__|_____|__|
 */
void tile_dualstack(monitor *m){
  pool *list = m->windows;
  int len = pool_count(list);

  int gaps = (m->gaps_enabled ? GAPS : 0);
  int cols = (len >= 3 ? 3 : len);

  float subcol_width_exact = m->width * STACK_RATIO;
  int subcol_width = round_float(subcol_width_exact);

  int win_right = closest_even(len) / 2;
  int win_left  = closest_odd(len)  / 2;

  float subcol_height_right_exact = 
    (len > 1 ?                              /* Return zero if right column does not need to exist */
      (float)m->height / win_right :        /* Otherwise, window height is monitor height divided */
      0.0                                   /*   by half of the closest even number to the number */
    );                                      /*   of windows (essentially a complex way of writing */
                                            /*   the number of windows in the right column)       */
  int subcol_height_right = round_float(subcol_height_right_exact);

  float subcol_height_left_exact =
    (len > 2 ?                            /* Return zero if the left column does not need to exist */
     (float)m->height / win_left :        /* Otherwise, window height is monitor height divided by */
     0.0                                  /*   the number of windows in the left column, see above */
    );                                    /* A note: every even number of windows corresponds to a */
                                          /*   new window in the right column, whereas every odd # */
                                          /*   of windows (apart from 1) corresponds to a new left */
                                          /*   window                                              */
  int subcol_height_left = round_float(subcol_height_left_exact);

  int x = 0, y = 0,
      width = 0, height = 0;

  int i = 0,
      current_col = 0,
      current_row_right = 0,
      current_row_left = 0;

  Window win;

  int ind;

  if(len == 0){
    return;
  }

  pool_foreach_nodecl(list){
    win = (Window)pool_get(ind, list);

    switch(current_col){
      case 0:
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
      case 1: /* This is really horrible */
        x = m->x + (m->width - subcol_width);
        y = m->y + (subcol_height_right * (current_row_right)) + (current_row_right > 0 ? 0 : gaps);
        width = subcol_width - gaps;
        height = subcol_height_right - (current_row_right > 0 ? gaps : gaps * 2);
        current_row_right++;
        break;
      case 2:
        x = m->x + gaps;
        y = m->y + (subcol_height_left * (current_row_left)) + (current_row_left > 0 ? 0 : gaps);
        width = subcol_width - gaps;
        height = subcol_height_left - (current_row_left > 0 ? gaps : gaps * 2);
        current_row_left++;
        break;
    }
    
    if(m->fullscreen_enabled &&
       m->fullscreen == win &&
       !above_enabled){
      x = m->x;
      y = m->y;
      width = m->width;
      height = m->height;

      XRaiseWindow(dpy, win);
    }

    x_move_resize(
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

#endif

#ifdef ENABLE_FIBONACCI

/*
 *  ___________
 * |     |  1  |
 * |  M  |     |
 * |     |_____|
 * |     | 2| 3|
 * |_____|__|__|
 */
void tile_fibonacci(monitor *m){
  int gaps = (m->gaps_enabled ? GAPS : 0);

  int x = gaps,
      y = gaps,
      w = m->width - 2 * gaps,
      h = m->height - 2 * gaps;

  Window win;

  pool_foreach(m->windows){
    win = (Window)pool_get(ind, m->windows);

    if(ind % 2){
      h = h / 2 - gaps / 2;
    } else {
      w = w / 2 - gaps / 2;
    }

    if(ind > 0){
      if(ind % 2){
        x += w + gaps;
      } else {
        y += h + gaps;
      }
    }
    
    if(m->fullscreen_enabled &&
       m->fullscreen == win &&
       !above_enabled){
      x = 0;
      y = 0;
      w = m->width;
      h = m->height;

      XRaiseWindow(dpy, win);
    }

    x_move_resize(
      win,
      x + m->x, y + m->y,
      w, h
    );
  }
}

#endif

#ifdef ENABLE_BOTTOMSTACK

/*
 *  ----------- 
 * |     M     |
 * |           |
 * |-----------|
 * | 1 | 2 | 3 |
 * |___________|
 */
void tile_bottomstack(monitor *m){
  pool *list = m->windows;

  int x, y,
      w, h;

  int gaps = (m->gaps_enabled ? GAPS : 0),
      indx = 0;

  int subcol_height = (pool_count(list) > 0 ? m->height * STACK_RATIO : 0);
  int subcol_width  = (pool_count(list) > 0 ? m->width / (pool_count(list) - 1) : 0);

  Window win;

  pool_foreach(list){
    win = (Window)pool_get(ind, list);

    if(indx == 0){
      x = m->x + gaps;
      y = m->y + gaps;
      w = m->width - (gaps * 2);
      h = m->height - (gaps * 2) - subcol_height;
    } else {
      x = m->x + (indx == 1 ? gaps : 0) + (subcol_width * (indx - 1));
      y = m->y + m->height - subcol_height;
      w = subcol_width - (indx == 1 ? gaps * 2 : gaps);
      h = subcol_height - gaps;
    }

    if(m->fullscreen_enabled &&
       m->fullscreen == win &&
       !above_enabled){
      x = m->x;
      y = m->y;
      w = m->width;
      h = m->height;

      XRaiseWindow(dpy, win);
    }

    x_move_resize(
      win,
      x, y,
      w, h
    );

    indx++;
  }
}

#endif

#endif
