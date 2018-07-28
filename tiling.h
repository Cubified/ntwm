/*
 * tiling.h: tiling utilities
 */

#ifndef __TILING_H
#define __TILING_H

static void tile();
static void tile_monitor(monitor *m);
static void toggle_gaps(monitor *current_monitor);
static void toggle_fullscreen(monitor *current_monitor, Window win);

void tile(){
  list_foreach_noroot(monitors){
    monitor *m = itr->data;
    tile_monitor(m);
  }
}

void tile_monitor(monitor *m){
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

#endif
