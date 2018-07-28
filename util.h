/*
 * util.h: various utility functions
 */

#ifndef __UTIL_H
#define __UTIL_H

#define LENGTH(x) (sizeof(x)/sizeof(x[0])) // Borrowed from dwm

enum {
  atom_protocols,
  atom_delete
};

static void init();
static void quit();
static void establish_keybinds(Window win);
static void spawn(const char *cmd);
static void select_input(Window win);
static void kill_focused();
static void move_resize(Window win, int x, int y, int w, int h);
static void sighandler(int signo);
static void setup_atoms();
static void set_cursor(unsigned int cursor_name);
static int send_event(Window win, Atom atom);
static int on_x_error(Display *d, XErrorEvent *e);

void init(){
  dpy = XOpenDisplay(0x0);
  if(dpy == NULL){
    error("Failed to open display.");
    quit();
  }

  root = DefaultRootWindow(dpy);

  XSetErrorHandler(&on_x_error);

  select_input(root);
  establish_keybinds(root);

  setup_atoms();
  
  set_cursor(XC_arrow);

  signal(SIGINT, sighandler);
}

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

  int success = send_event(focused,atoms[atom_delete]);

  if(!success){
    XGrabServer(dpy);
    XSetCloseDownMode(dpy,DestroyAll);
    XKillClient(dpy,focused);
    XSync(dpy,false);
    XUngrabServer(dpy);
  }
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

void setup_atoms(){
  atoms[atom_protocols] = XInternAtom(dpy,"WM_PROTOCOLS",false);
  atoms[atom_delete] = XInternAtom(dpy,"WM_DELETE_WINDOW",false);
}

void set_cursor(unsigned int cursor_name){
  Cursor cursor = XCreateFontCursor(dpy, cursor_name);
  XDefineCursor(dpy,root,cursor);
}

int send_event(Window win, Atom atom){
  int protocols_count;
  Atom *protocols;
  XEvent evt;

  int exists = 0;

  XGetWMProtocols(dpy,win,&protocols,&protocols_count);

  while(!exists){
    exists = (protocols[protocols_count] == atom);
    protocols_count--;
  }

  XFree(protocols);

  if(exists){
    evt.type = ClientMessage;
    evt.xclient.window = win;
    evt.xclient.message_type = atoms[atom_protocols];
    evt.xclient.format = 32;
    evt.xclient.data.l[0] = atom;
    evt.xclient.data.l[1] = CurrentTime;
    XSendEvent(dpy,win,false,NoEventMask,&evt);
  }

  return exists;
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
