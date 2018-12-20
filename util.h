/*
 * util.h: various utility functions
 */

#ifndef __UTIL_H
#define __UTIL_H

#define LENGTH(x) (sizeof(x)/sizeof(x[0])) // Borrowed from dwm

enum {
  atom_protocols,
  atom_delete,
  atom_type,
  atom_name,
  atom_check
};

enum {
  window_normal,
  window_dock,
  window_dialog,
  window_splash,
  window_taskbar,
  window_utility
};

static void init();
static void quit();
static void reset();
static void establish_keybinds(Window win);
static void spawn(const char *cmd);
static void select_input(Window win);
static void kill_focused();
static void move_resize(Window win, int x, int y, int w, int h);
static void set_cursorpos(int x, int y);
static void sighandler(int signo);
static void setup_atoms();
static void set_cursor(unsigned int cursor_name);
static void set_focused(Window win);
static int send_event(Window win, Atom atom);
static int on_x_error(Display *d, XErrorEvent *e);
static int round_float(float n);
static int closest_even(int n);
static int closest_odd(int n);
static int valid_window(Window win);
static int window_gettype(Window win);
static int is_child(Window win);

/*
 * Initialize the connection
 * to the X server, among a
 * few other variables
 */
void init(){
  dpy = XOpenDisplay(0x0);
  if(dpy == NULL){
    error("Failed to open display.");
    quit();
    return;
  }

  root = DefaultRootWindow(dpy);
  screen = DefaultScreenOfDisplay(dpy);
  focused = 0;
  bars = list_init();

  XSetErrorHandler(&on_x_error);

  select_input(root);
  establish_keybinds(root);

  setup_atoms();

  XChangeProperty(dpy,root,atoms[atom_check],XA_WINDOW,32,PropModeReplace,(unsigned char *)&root,1);
  XChangeProperty(dpy,root,atoms[atom_name],XA_STRING,8,PropModeReplace,(unsigned char *)"ntwm",4);
  
  set_cursor(XC_arrow);

  signal(SIGINT, sighandler);
}

/*
 * Quit ntwm (Note: requires
 * a new X event to terminate
 * the event loop, fairly
 * unimportant because this
 * function is only called
 * on SIGINT
 */
void quit(){
  running = false;
}

/*
 * Forks a child process,
 * used with the "spawn"
 * command in config.h
 */
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

/*
 * Grab input events from a window,
 * used on map events as well as
 * init()
 */
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

/*
 * Grab desired events from a window,
 * used in conjunction with
 * establish_keybinds()
 */
void select_input(Window win){
  XSelectInput(
    dpy,
    win,
    EnterWindowMask |
    SubstructureRedirectMask |
    SubstructureNotifyMask
  );
}

/*
 * Kills the focused window,
 * used with a keybind in
 * config.h
 */
void kill_focused(){
  if(valid_window(focused)){
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
}

/*
 * Moves and resizes a window
 */
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

/*
 * Sets the position of the cursor
 * onscreen, used when moving a
 * window to a new monitor and
 * upon window map
 */
void set_cursorpos(int x, int y){
  XWarpPointer(
    dpy,
    None,
    root,
    0, 0,
    0, 0,
    x, y
  );
  XFlush(dpy);
}

/*
 * Quits ntwm on SIGINT
 */
void sighandler(int signo){
  quit();
}

/*
 * Sets up atoms for later use
 * with kill_focused()
 */
void setup_atoms(){
  atoms[atom_protocols] = XInternAtom(dpy,"WM_PROTOCOLS",false);
  atoms[atom_delete] = XInternAtom(dpy,"WM_DELETE_WINDOW",false);
  atoms[atom_type] = XInternAtom(dpy,"_NET_WM_WINDOW_TYPE",false);
  atoms[atom_name] = XInternAtom(dpy,"_NET_WM_NAME",false);
  atoms[atom_check] = XInternAtom(dpy,"_NET_SUPPORTING_WM_CHECK",false);
}

/*
 * Creates (and sets) the cursor,
 * used in init() to ensure the
 * default arrow cursor is
 * visible
 */
void set_cursor(unsigned int cursor_name){
  Cursor cursor = XCreateFontCursor(dpy, cursor_name);
  XDefineCursor(dpy,root,cursor);
}

/*
 * Sets the currently focused
 * window, both through the
 * "focused" variable as
 * well as through the X
 * server
 */
void set_focused(Window win){
  focused = win;
  XSetInputFocus(dpy,win,RevertToParent,CurrentTime);
}

/*
 * Sends an event to a
 * window using an
 * atom, used with
 * kill_focused()
 */
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

/*
 * X server error handler
 * (Note: does not quit
 * on non-fatal error)
 */
int on_x_error(Display *d, XErrorEvent *e){
  if(e->error_code == 10){
    if(!has_thrown){
      error("Another window manager is already running.");
      has_thrown = true;
      quit();
    }
  } else {
    error("%s (error code %i)",last_call,e->error_code);
  }
  last_err = e->error_code;
  return 0;
}

/*
 * Rounds a float to
 * the nearest int
 */
int round_float(float n){
  return (int)(n < 0 ? n - 0.5 : n + 0.5);
}

/*
 * Finds the closest
 * even number to
 * an integer n
 */
int closest_even(int n){
  return (n % 2 == 0 ? n : n - 1);
}

/*
 * Finds the closest
 * odd number to
 * an integer n
 */
int closest_odd(int n){
  return (n % 2 ? n : n - 1);
}

/*
 * Checks if the focused window
 * has not been destroyed and
 * is not the root window
 */
int valid_window(Window win){
  return (focused != 0 && focused != root);
}

/*
 * Returns 1 if _NET_WM_WINDOW_TYPE
 * equals _NET_WM_WINDOW_TYPE_DOCK
 * (and 0 otherwise)
 */
int window_gettype(Window win){
  Atom          real;
  int           fmt;
  unsigned long extra;
  unsigned long n_items;
  unsigned char *prop;

  int success = XGetWindowProperty(
    dpy,
    win,
    atoms[atom_type],
    0L,
    2L,
    false,
    AnyPropertyType,
    &real,
    &fmt,
    &n_items,
    &extra,
    &prop
  );

  if(success == Success && prop != NULL){
    char *name = XGetAtomName(dpy,*(Atom*)prop);
    if(strcmp(name,"_NET_WM_WINDOW_TYPE_DOCK") == 0){
      return window_dock;
    } else if(strcmp(name,"_NET_WM_WINDOW_TYPE_DIALOG") == 0){
      return window_dialog;
    } else if(strcmp(name,"_NET_WM_WINDOW_TYPE_SPLASH") == 0){
      return window_splash;
    } else if(strcmp(name,"_NET_WM_WINDOW_TYPE_TASKBAR") == 0){
      return window_taskbar;
    } else if(strcmp(name,"_NET_WM_WINDOW_TYPE_UTILITY") == 0){
      return window_utility;
    }
  }
  return window_normal;
}

/*
 * Returns 1 if the window is
 * a child of a window other
 * than root, 0 otherwise
 */
int is_child(Window win){
  Window root,
         parent,
         *children;
  unsigned int count;
  
  XQueryTree(
    dpy,
    win,
    &root,
    &parent,
    &children,
    &count
  );

  return (parent != root);
}

#endif
