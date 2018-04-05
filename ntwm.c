/*
 * ntwm.c: ntwm core
 */

#include <stdbool.h>
#include <string.h>
#include <unistd.h>

#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>

#include "config.h"
#include "dynam.h"
#include "logging.h"

bool running = true;
bool has_thrown = false;

const char *last_call = "";

int last_err = 0;
int monitor_width;
int monitor_height;
int gaps_enabled;

Display *dpy;
XEvent e;
Window focused;
Screen *screen;

#include "util.h"
#include "multihead.h"
#include "tiling.h"
#include "events.h"

int main(){
  info("ntwm v" VER " starting up.");

  dpy = XOpenDisplay(0x0);
  screen = XDefaultScreenOfDisplay(dpy);

  gaps_enabled = 1;

  if(dpy == NULL){
    error("Failed to open display.\n");
    quit();
    return 1;
  }

  XSetErrorHandler(&on_x_error);

  select_input(DefaultRootWindow(dpy));
  establish_keybinds(DefaultRootWindow(dpy));

  XSync(dpy,false);
    
  multihead_setup();
    
  if(!last_err){
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
    }
  }

  info("ntwm shutting down.");

  XCloseDisplay(dpy);

  multihead_free();

  return last_err;
}
