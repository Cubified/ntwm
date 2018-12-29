/*
 * multihead.h: multimonitor support
 */

#ifndef __MULTIHEAD_H
#define __MULTIHEAD_H

typedef struct monitor {
  node *windows;
  int width;
  int height;
  int x;
  int y;
  int mode;
  bool gaps_enabled;
  bool fullscreen_enabled;
  Window fullscreen;
} monitor;

static void multihead_setup();
static void multihead_free();
static void multihead_resize();
static monitor *find_monitor();

static void multihead_addbar(Window win);
static void multihead_delbar(Window win);
static int  multihead_isbar(Window win);
static monitor *monitor_atpos(int x, int y);

#ifdef MULTIHEAD

/*
 * Creates a linked list of monitors
 * using Xrandr configuration
 */
void multihead_setup(){
  int i;
  XRRScreenResources *screen_resources = XRRGetScreenResources(dpy,root);

  monitors = list_init();
 
  for(i=0;i<screen_resources->ncrtc;i++){
    XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(dpy,screen_resources,screen_resources->crtcs[i]);

    if(crtc_info->width > 0){
      monitor *mon = malloc(sizeof(monitor));
      node *n = list_push(monitors);

      mon->windows = list_init();
      mon->width = crtc_info->width;
      mon->height = crtc_info->height;
      mon->x = crtc_info->x;
      mon->y = crtc_info->y;
      mon->mode = 0;
      mon->gaps_enabled = true;
      mon->fullscreen_enabled = false;
      mon->fullscreen = 0;

      n->data = mon;
    }

    XRRFreeCrtcInfo(crtc_info);
  }
  XSync(dpy,false);
  XRRFreeScreenResources(screen_resources);

  XRRQueryExtension(dpy,&rr_event_base,&rr_error_base);
  
  XRRSelectInput(
    dpy,
    root,
    RRScreenChangeNotifyMask
  );
}

/*
 * Frees the linked list of monitors,
 * as well as each of the window
 * linked lists inside the
 * monitor struct
 */
void multihead_free(){
  list_foreach(monitors){
    monitor *m = (monitor*)itr->data;
    if(m != NULL && m->windows != NULL){
      list_free(m->windows);
    }
  }
  list_free(monitors);
  list_free(bars);
}

/*
 * Updates monitor linked list
 * in the same manner as
 * multihead_setup()
 */
void multihead_resize(){
  XRRScreenResources *screen_resources = XRRGetScreenResources(dpy,root);
  XRRCrtcInfo *crtc_info;
  monitor *m;
  int i = 0;
  list_foreach_noroot(monitors){
    m = itr->data;
    crtc_info = XRRGetCrtcInfo(dpy,screen_resources,screen_resources->crtcs[i]);
    m->width = crtc_info->width;
    m->height = crtc_info->height;
    m->x = crtc_info->x;
    m->y = crtc_info->y;
    i++;
  }
}

/*
 * Finds the currently focused monitor
 * based on the cursor's position
 */
monitor *find_monitor(){
  Window win;
  int pos_x, pos_y,
    win_x, win_y;
  unsigned int mask_return;
  XQueryPointer(
    dpy,
    root,
    &win,
    &win,
    &pos_x,
    &pos_y,
    &win_x,
    &win_y,
    &mask_return
  );

  return monitor_atpos(pos_x,pos_y);
}

#endif

#ifndef MULTIHEAD

/*
 * Because multihead is not enabled,
 * simply creates a single node/
 * monitor that is the size of
 * the display in X
 */
void multihead_setup(){
  monitors = list_init();
  monitor *monitor = malloc(sizeof(monitor));
  monitor->windows = list_init();
  monitor->width = XWidthOfScreen(screen);
  monitor->height = XHeightOfScreen(screen);
  monitor->x = 0;
  monitor->y = 0;
  monitor->mode = 0;
  monitor->gaps_enabled = true;
  monitor->fullscreen_enabled = false;
  monitor->fullscreen = 0;
  rr_event_base = -1;
  rr_error_base = -1;
  node *n = list_push(monitors);
  n->data = monitor;
}

/*
 * Frees this single node, as well as
 * the windows it holds
 */
void multihead_free(){
  list_foreach_noroot(monitors){
    monitor *m = (monitor*)itr->data;
    if(m != NULL && m->windows != NULL){
      list_free(m->windows);
    }
  }
  list_free(monitors);
  list_free(bars);
}

/*
 * Should never be called,
 * but updates monitor
 * size without using
 * Xrandr
 */
void multihead_resize(){
  monitor *m;
  list_foreach_noroot(monitors){
    m = (monitor*)itr->data;
    m->width = XWidthOfScreen(screen);
    m->height = XHeightOfScreen(screen);
  }
}

/*
 * Returns the one (and only)
 * monitor
 */
monitor *find_monitor(){
  return list_get(monitors,1)->data;
}

#endif

/*
 * Reduces the size of the
 * current monitor to make
 * room for a taskbar,
 * moving it down if
 * necessary
 */
void multihead_addbar(Window win){ 
  Window root_ofwin;
  int x, y;
  unsigned int w, h,
    borderwidth,
    depth;
  monitor *m;
  node *newbar;

  XGetGeometry(
    dpy,
    win,
    &root_ofwin,
    &x, &y,
    &w, &h,
    &borderwidth,
    &depth
  ); 

  m = monitor_atpos(x,y);

  newbar = list_push(bars);
  newbar->data = malloc(sizeof(int));
  *(int*)newbar->data = h;
  newbar->data_noptr = win;
  
  m->height -= h;

  /*
   * Bar is most likely a topbar,
   * move the monitor's
   * available space
   * down
   */
  if(y <= (m->height+m->y) / 2){
    m->y += h;
    /*
     * Misuse of this member
     * to store this
     * information
     */
    newbar->size = 1;
  }
}

/*
 * Does the opposite of
 * multihead_addbar()
 */
void multihead_delbar(Window win){
  monitor *m = find_monitor();

  node *bar = list_find(bars, NULL, win);
  if(bar != NULL){
    m->height += *(int*)bar->data;
    if(bar->size){
      m->y -= *(int*)bar->data;
    }
    list_pop(bars, bar);
  }
}

/*
 * Returns 1 if a given
 * window is a bar (and
 * therefore should not
 * be re-mapped with
 * tile_existing())
 */
int multihead_isbar(Window win){
  return (list_find(bars, NULL, win) != NULL);
}

/*
 * Returns which monitor contains
 * a given point
 */
monitor *monitor_atpos(int x, int y){
  int mon_x, mon_y,
      mon_w, mon_h;
  monitor *m = monitors->next->data;

  list_foreach_noroot(monitors){
    m = (monitor*)itr->data;
    mon_x = m->x;
    mon_y = m->y;
    mon_w = m->width;
    mon_h = m->height;

    if(x <= mon_x + mon_w &&
       x >= mon_x &&
       y <= mon_y + mon_h &&
       y >= mon_y){
      break;
    }
  }

  return m;
}

#endif
