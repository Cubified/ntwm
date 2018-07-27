/*
 * util.h: various utility functions
 */

#ifndef __UTIL_H
#define __UTIL_H

#define LENGTH(x) (sizeof(x)/sizeof(x[0])) // Borrowed from dwm

static void quit();
static void establish_keybinds(Window win);
static void spawn(const char *cmd);
static void select_input(Window win);
static void kill_focused();
static void move_resize(Window win, int x, int y, int w, int h);
static void sighandler(int signo);
static int on_x_error(Display *d, XErrorEvent *e);

void quit(){
  running = false;
}

void spawn(const char *cmd){
  pid_t process = fork();

  if(process < 0){
    error("Could not fork process.");
  } else if(process == 0){
    char *argv[] = {(char *)cmd, NULL};
    int retval = execvp(cmd,argv);
    if(retval == -1){
      error("Failed to start process.");
    }
  }
}

void establish_keybinds(Window win){
  for(int i=0;i<LENGTH(keys);i++){
    XGrabKey(
      dpy,
      XKeysymToKeycode(dpy,keys[i].keysym),
      keys[i].mod,
      win,
      false,
      GrabModeAsync,
      GrabModeAsync
    );
  }
}

void select_input(Window win){
  XSelectInput(
    dpy,
    win,
    EnterWindowMask |
    SubstructureRedirectMask |
    SubstructureNotifyMask
  );
}

void kill_focused(){
  XUnmapWindow(dpy,focused);
}

void move_resize(Window win, int x, int y, int w, int h){
  XWindowChanges wc;

  wc.x = x;
  wc.y = y;
  wc.width = w;
  wc.height = h;

  XConfigureWindow(
    dpy,
    win,
    CWX|CWY|CWWidth|CWHeight,
    &wc
  );
}

void sighandler(int signo){
  quit();
}

int on_x_error(Display *d, XErrorEvent *e){
  if(e->error_code == 10){
    if(!has_thrown){
      error("Another window manager is already running.");
      has_thrown = true;
    }
  } else {
    if(log_colors){
      printf(ANSI_COLOR_RED "==> Error: %i" ANSI_COLOR_RESET "\n", e->error_code);
    } else {
      printf("==> Error: %i\n", e->error_code);
    }
    error(last_call);
  }
  quit();
  last_err = e->error_code;
  return 0;
}

#endif
