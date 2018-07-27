/*
 * ntwm.c: ntwm core
 */

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
#include <X11/extensions/Xrandr.h>

#include "config.h"
#include "logging.h"
#include "list.h"

bool running = true;
bool has_thrown = false;

char *last_call = "";

int last_err = 0;
int monitor_width;
int monitor_height;
int rr_event_base;
int rr_error_base;

Display *dpy;
XEvent e;
Window focused;
Screen *screen;

node *monitors;

#include "util.h"
#include "multihead.h"
#include "tiling.h"
#include "events.h"

int main(){
  info("ntwm v" VER " starting up.");

  dpy = XOpenDisplay(0x0);

  if(dpy == NULL){
    error("Failed to open display.");
    quit();
    return 1;
  }

  screen = XDefaultScreenOfDisplay(dpy);
  if(screen == NULL){
    error("Failed to detect default screen.");
    quit();
    return 1;
  }

  XSetErrorHandler(&on_x_error);

  select_input(DefaultRootWindow(dpy));
  establish_keybinds(DefaultRootWindow(dpy));

  if(!last_err){
    multihead_setup();
    spawn(autostartcmd);
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