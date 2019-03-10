/*
 * x.h: utility functions for interacting with X
 */

#ifndef __X_H
#define __X_H

enum {
  atom_protocols,
  atom_delete,
  atom_type,
  atom_name,
  atom_check,
  atom_state,
  atom_above,
  atom_fullscreen
};

/*
 * hash_str("_NET_WM_WINDOW_TYPE_X")
 * (Better solutions may exist)
 */
enum {
  window_normal = 873,
  window_dock = 801,
  window_dialog = 848,
  window_splash = 875,
  window_taskbar = 888,
  window_utility = 932
};

static void x_init();
static void x_establish_keybinds(Window win);
static void x_select_input(Window win);
static void x_kill_focused();
static void x_move_resize(Window win, int x, int y, int w, int h);
static void x_set_property(Atom atom, Atom type, int size, unsigned char *data);
static void x_set_cursorpos(int x, int y);
static void x_sighandler(int signo);
static void x_setup_atoms();
static void x_set_cursor(unsigned int cursor_name);
static void x_set_focused(Window win);
static void x_query_window(Window win, int *x, int *y, unsigned int *w, unsigned int *h);
static int x_send_event(Window win, Atom atom);
static int on_x_error(Display *dpy, XErrorEvent *e);
static int x_valid_window(Window win);
static int x_window_gettype(Window win);
static int x_is_child(Window win);

/*
 * Initialize the connection
 * to the X server, among a
 * few other variables
 */
void x_init(){
  dpy = XOpenDisplay(0x0);
  if(dpy == NULL){
    error("Failed to open display.");
    quit();
    return;
  }

  root = DefaultRootWindow(dpy);
  screen = DefaultScreenOfDisplay(dpy);
  focused = 0;
  above = 0;
  bars = pool_init(MAX_BARS);

  XSetErrorHandler(&on_x_error);

  x_select_input(root);
  x_establish_keybinds(root);

  x_setup_atoms();

  x_set_property(atoms[atom_check], XA_WINDOW, 32, (unsigned char *)&root);
  x_set_property(atoms[atom_name],  XA_STRING, 8,  (unsigned char *)"ntwm");

  x_set_cursor(XC_arrow);

  signal(SIGINT,  x_sighandler);
  signal(SIGTERM, x_sighandler);
  signal(SIGQUIT, x_sighandler);
}

void x_establish_keybinds(Window win){
  int i;
  for(i=0;i<LENGTH(keys);i++){
    XGrabKey(
      dpy,
      XKeysymToKeycode(dpy, keys[i].keysym),
      keys[i].mod,
      win,
      0,
      GrabModeAsync,
      GrabModeAsync
    );
  }
}

/*
 * Grab desired events from a window,
 * used in conjunction with
 * x_establish_keybinds()
 */
void x_select_input(Window win){
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
void x_kill_focused(){
  if(x_valid_window(focused)){
    int success = x_send_event(focused, atoms[atom_delete]);
    
    XUnmapWindow(dpy, focused);

    if(!success){
      XGrabServer(dpy);
      XSetCloseDownMode(dpy, DestroyAll);
      XKillClient(dpy, focused);
      XSync(dpy, 0);
      XUngrabServer(dpy);
    }
  }
}

/*
 * Moves and resizes a window
 */
void x_move_resize(Window win, int x, int y, int w, int h){
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
 * Sets an Xlib property
 * using a given atom
 */
void x_set_property(Atom atom, Atom type, int size, unsigned char *data){
  XChangeProperty(dpy, root, atom, type, size, PropModeReplace, data, 1);
}

/*
 * Sets the position of the cursor
 * onscreen, used when moving a
 * window to a new monitor and
 * upon window map
 */
void x_set_cursorpos(int x, int y){
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
 * Quits ntwm on relevant
 * signals, sending a
 * dummy event for
 * XNextEvent()
 */
void x_sighandler(int signo){
  XEvent evt;

  quit();

  evt.type = EnterNotify;

  XSendEvent(
    dpy,
    root,
    False,
    EnterWindowMask,
    &evt
  );
  XFlush(dpy);
}

/*
 * Sets up atoms for later use,
 * TODO: Add support for more
 */
void x_setup_atoms(){
  atoms[atom_protocols]  = XInternAtom(dpy, "WM_PROTOCOLS", 0);
  atoms[atom_delete]     = XInternAtom(dpy, "WM_DELETE_WINDOW", 0);
  atoms[atom_type]       = XInternAtom(dpy, "_NET_WM_WINDOW_TYPE", 0);
  atoms[atom_name]       = XInternAtom(dpy, "_NET_WM_NAME", 0);
  atoms[atom_check]      = XInternAtom(dpy, "_NET_SUPPORTING_WM_CHECK", 0);
  atoms[atom_state]      = XInternAtom(dpy, "_NET_WM_STATE", 0);
  atoms[atom_above]      = XInternAtom(dpy, "_NET_WM_STATE_ABOVE", 0);
  atoms[atom_fullscreen] = XInternAtom(dpy, "_NET_WM_STATE_FULLSCREEN", 0);
}

/*
 * Creates (and sets) the cursor,
 * used in x_init() to ensure the
 * default arrow cursor is
 * visible
 */
void x_set_cursor(unsigned int cursor_name){
  Cursor cursor = XCreateFontCursor(dpy, cursor_name);
  XDefineCursor(dpy, root, cursor);
}

/*
 * Sets the currently focused
 * window, both through the
 * "focused" variable as
 * well as through the X
 * server
 */
void x_set_focused(Window win){
  focused = win;
  XSetInputFocus(dpy, win, RevertToParent, CurrentTime);
}

/*
 * Returns geometry information
 * about a window
 */
void x_query_window(Window win, int *x, int *y, unsigned int *w, unsigned int *h){
  Window root_notused;
  unsigned int bw_notused, depth_notused;
  XGetGeometry(
    dpy,
    win,
    &root_notused,
    x, y,
    w, h,
    &bw_notused,
    &depth_notused
  );
}

/*
 * Sends a ClientMessage event
 * to a given window
 */
int x_send_event(Window win, Atom atom){
  int protocols_count = 0,
      exists = 0;
  Atom *protocols;
  XEvent evt;

  XGetWMProtocols(dpy, win, &protocols, &protocols_count);

  protocols_count--;

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
    XSendEvent(dpy, win, 0, NoEventMask, &evt);
  }

  return exists;
}

/*
 * X server error handler
 * (Note: does not quit
 * on non-fatal error)
 */
int on_x_error(Display *dpy, XErrorEvent *e){
  if(e->error_code == 10){
    if(!has_thrown){
      error("Another window manager is already running.");
      has_thrown = 1;
      quit();
    }
  } else {
#ifdef LOGGING_NO_STDIO
    error(last_call);
#else
    error("%s (error code %i)", last_call, e->error_code);
#endif
  }
  last_err = e->error_code;
  return 0;
}

/*
 * Checks if the focused window
 * has not been destroyed and
 * is not the root window
 */
int x_valid_window(Window win){
  return (focused != 0 && focused != root);
}

/*
 * Returns 1 if _NET_WM_WINDOW_TYPE
 * equals _NET_WM_WINDOW_TYPE_DOCK
 * (and 0 otherwise)
 */
int x_window_gettype(Window win){
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
    0,
    AnyPropertyType,
    &real,
    &fmt,
    &n_items,
    &extra,
    &prop
  );

  if(success == Success && prop != NULL){
    char *name = XGetAtomName(dpy, *(Atom*)prop);
    int hash = hash_str(name);

    free(prop);
    free(name);

    return hash;
  }
  
  free(prop);

  return window_normal;
}

/*
 * Returns 1 if the window is
 * a child of a window other
 * than root, 0 otherwise
 */
int x_is_child(Window win){
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

  XFree(children);

  return (parent != root);
}


#endif
