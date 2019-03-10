/*
 * ntwm.c: ntwm core
 */

#include <stdlib.h>
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
#include "libspool.h"

int running = 1;
int has_thrown = 0;
int above_enabled = 0;

char *last_call = "";

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
Window above;
Atom atoms[8];

pool *monitors;
pool *bars;

#include "util.h"
#include "x.h"
#include "multihead.h"
#include "modes.h"
#include "tiling.h"
#include "events.h"

int main(){
  info("ntwm v" VER " starting up.");

  x_init();

  if(!last_err && running){
    multihead_setup();
    spawn(autostartcmd);
    tile_existing(map_request);
  }
 
  while(running){
    XNextEvent(dpy, &e);
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
      case ClientMessage:
        client_message(&e);
        break;
      default:
        if(e.type == rr_event_base + RRScreenChangeNotify){
          screenchange_notify(&e);
        }
        break;
    }
  }

  info("ntwm shutting down.");

  multihead_free();

  return last_err;
}
