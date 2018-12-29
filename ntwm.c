/*
 * ntwm.c: ntwm core
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <X11/cursorfont.h>
#include <X11/extensions/Xrandr.h>

#include "config.h"
#include "logging.h"
#include "list.h"

bool running = true;
bool has_thrown = false;

char *last_call = "";

long event_mask;

int last_err = 0;
int mode_index = 0;
int monitor_width;
int monitor_height;
int rr_event_base;
int rr_error_base;

Display *dpy;
Screen *screen;
XEvent e;
Window focused;
Window root;
Atom atoms[5];

node *monitors;
node *bars;

#include "util.h"
#include "multihead.h"
#include "modes.h"
#include "tiling.h"
#include "events.h"

int main(){
  info("ntwm v" VER " starting up.");

  init();

  if(!last_err && running){
    multihead_setup();
    spawn(autostartcmd);
    tile_existing(map_request);
  }
 
  while(running){
    if(XCheckMaskEvent(
        dpy,
        event_mask,
        &e
      )){
      switch(e.type){
        case MapRequest:
          map_request(&e);
          break;
        case ConfigureRequest:
          configure_request(&e);
          break;
        case UnmapNotify:
          unmap_notify(&e);
          break;
        case EnterNotify:
          enter_notify(&e);
          break;
        case KeyPress:
          key_press(&e);
          break;
      }
    } else if(XCheckTypedEvent(
      dpy,
      rr_event_base + RRScreenChangeNotify,
      &e
    )){
      screenchange_notify(&e);
    }
  }

  info("ntwm shutting down.");

  multihead_free();

  return last_err;
}
