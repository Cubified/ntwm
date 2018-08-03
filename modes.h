/*
 * modes.h: tiling modes
 */

#ifndef __MODES_H
#define __MODES_H

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

/*
 *  ___________
 * | 2|     | 1|
 * |__|  M  |__|
 * | 4|     | 3|
 * |__|_____|__|
 */
void tile_dualstack(monitor *m){
  node *list = m->windows;
  int len = list_sizeof(list);

  if(len == 0){
    return;
  }

  int gaps = (m->gaps_enabled ? GAPS : 0);
  int cols = (len >= 3 ? 3 : len);

  float subcol_width_exact = m->width * GRID_RATIO;
  int subcol_width = round_float(subcol_width_exact);

  int win_right = closest_even(len) / 2;
  int win_left  = closest_odd(len)  / 2;

  float subcol_height_right_exact = 
    (len > 1 ?                              // Return zero if right column does not need to exist
      (float)m->height / win_right :        // Otherwise, window height is monitor height divided
      0.0                                   //   by half of the closest even number to the number
    );                                      //   of windows (essentially a complex way of writing
                                            //   the number of windows in the right column)
  int subcol_height_right = round_float(subcol_height_right_exact);

  float subcol_height_left_exact =
    (len > 2 ?                            // Return zero if the left column does not need to exist
     (float)m->height / win_left :        // Otherwise, window height is monitor height divided by
     0.0                                  //   the number of windows in the left column, see above
    );                                    // A note: every even number of windows corresponds to a
                                          //   new window in the right column, whereas every odd #
                                          //   of windows (apart from 1) corresponds to a new left
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

  int ind = 0,
      x = gaps,
      y = gaps,
      w = m->width - 2 * gaps,
      h = m->height - 2 * gaps;

  list_foreach_noroot(m->windows){
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
    
    if(m->fullscreen_enabled && m->fullscreen == itr->data_noptr){
      x = m->x;
      y = m->y;
      w = m->width;
      h = m->height;

      XRaiseWindow(dpy,itr->data_noptr);
    }

    move_resize(
      itr->data_noptr,
      x, y,
      w, h
    );

    ind++;
  }
}

#endif
