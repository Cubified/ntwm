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
static int round_float(float n);
static int closest_even(int n);
static int closest_odd(int n);
static int valid_window(Window win);

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
  }

  root = DefaultRootWindow(dpy);
  screen = DefaultScreenOfDisplay(dpy);
  focused = 0;

  XSetErrorHandler(&on_x_error);

  select_input(root);
  establish_keybinds(root);

  setup_atoms();
  
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
 * Extremely complex functionality:
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

#endif
