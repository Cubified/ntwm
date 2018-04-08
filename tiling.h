/*
 * tiling.h: tiling utilities
 */

#ifndef __TILING_H
#define __TILING_H

static void tile(list_node *list);
static void toggle_gaps(monitor *current_monitor);
static void toggle_fullscreen(monitor *current_monitor, Window win);

void tile(list_node *list){
  int clients_count, i,
    cols, rows,
    col_height, col_width,
    col_number, row_number,
    x, y,
    w, h,
    gaps;
  monitor *current_monitor;

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
  current_monitor = find_monitor();

  if(current_monitor == NULL){
    error("Could not locate monitor.");
  }

  gaps = (current_monitor->gaps_enabled ? GAPS : 0);

  col_height = current_monitor->height - gaps;
  col_width = (current_monitor->width - gaps) / (cols ? cols : 1);
  col_number = 0;
  row_number = 0;
  i = 0;

  LIST_NODE_FOREACH_NOROOT(list){
    if(i / rows + 1 > cols - clients_count % cols){
      rows = clients_count / cols + 1;
    }

    x = (col_number * col_width) + gaps + current_monitor->x;
    y = row_number * col_height / rows + gaps + current_monitor->y;
    w = col_width - gaps;
    h = (col_height / rows) - gaps;

    if(current_monitor->fullscreen_enabled && current_monitor->fullscreen == iterator->window){
      x = 0;
      y = 0;
      w = current_monitor->width;
      h = current_monitor->height;

      XRaiseWindow(dpy,iterator->window);
    }

    move_resize(
      iterator->window,
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
