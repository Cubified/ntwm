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
  bool gaps_enabled;
  bool fullscreen_enabled;
  Window fullscreen;
} monitor;

static void multihead_setup();
static void multihead_free();
static void multihead_resize();
static monitor *find_monitor();

#ifdef MULTIHEAD

void multihead_setup(){
  monitors = list_init();
 
  XRRScreenResources *screen_resources = XRRGetScreenResources(dpy,root);
  for(int i=0;i<screen_resources->ncrtc;i++){
    XRRCrtcInfo *crtc_info = XRRGetCrtcInfo(dpy,screen_resources,screen_resources->crtcs[i]);

    if(crtc_info->width > 0){
      monitor *mon = malloc(sizeof(monitor));
      mon->windows = list_init();
      mon->width = crtc_info->width;
      mon->height = crtc_info->height;
      mon->x = crtc_info->x;
      mon->y = crtc_info->y;
      mon->gaps_enabled = true;
      mon->fullscreen_enabled = false;
      mon->fullscreen = 0;

      node *n = list_push(monitors);
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

void multihead_free(){
  list_foreach(monitors){
    monitor *m = (monitor*)itr->data;
    if(m != NULL && m->windows != NULL){
      list_free(m->windows);
    }
  }

  list_free(monitors);
}

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

  int mon_x, mon_y,
      mon_w, mon_h;
  monitor *m = (monitor*)monitors->next->data;

  list_foreach_noroot(monitors){
    m = (monitor*)itr->data;
    mon_x = m->x;
    mon_y = m->y;
    mon_w = m->width;
    mon_h = m->height;

    if(pos_x <= mon_x + mon_w &&
       pos_x >= mon_x &&
       pos_y <= mon_y + mon_h &&
       pos_y >= mon_y){
      break;
    }
  }

  return m; // Return a monitor, even if it is not the correct one
}

#endif

#ifndef MULTIHEAD

void multihead_setup(){
  monitors = list_init();
  monitor *monitor = malloc(sizeof(monitor));
  monitor->windows = list_init();
  monitor->width = XWidthOfScreen(screen);
  monitor->height = XHeightOfScreen(screen);
  monitor->x = 0;
  monitor->y = 0;
  monitor->gaps_enabled = true;
  monitor->fullscreen_enabled = false;
  monitor->fullscreen = 0;
  rr_event_base = -1;
  rr_error_base = -1;
  node *n = list_push(monitors);
  n->data = monitor;
}

void multihead_free(){
  list_foreach_noroot(monitors){
    monitor *m = (monitor*)itr->data;
    if(m != NULL && m->windows != NULL){
      list_free(m->windows);
    }
  }
  list_free(monitors);
}

void multihead_resize(){
  monitor *m;
  list_foreach_noroot(monitors){
    m = (monitor*)itr->data;
    m->width = XWidthOfScreen(screen);
    m->height = XHeightOfScreen(screen);
  }
}

monitor *find_monitor(){
  return list_get(monitors,1)->data;
}

#endif

#endif
