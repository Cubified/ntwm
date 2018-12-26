/*
 * config.h: ntwm configuration
 */

#define VER "1.1.2"
#define GAPS 50
#define STACK_RATIO 0.3
#define DIALOG_RATIO 0.7
#define MULTIHEAD /* multimonitor support */
#define MASK Mod1Mask
#define SHIFT ShiftMask

#define TERMCMD "xterm"

char *autostartcmd = "true";

typedef struct key {
  unsigned int mod;
  KeySym keysym;
  char *func;
  char *arg;
} key;

const key keys[] = {
/* Mask         Keysym      Function  Argument */
  {MASK|SHIFT,  XK_e,       "quit",   NULL},
  {MASK,        XK_Return,  "spawn",  TERMCMD},
  {MASK|SHIFT,  XK_q,       "kill",   NULL},
  {MASK,        XK_f,       "gaps",   NULL},
  {MASK|SHIFT,  XK_f,       "full",   NULL},
  {MASK|SHIFT,  XK_Right,   "next",   NULL},
  {MASK|SHIFT,  XK_Left,    "prev",   NULL},
  {MASK,        XK_o,       "mode",   NULL},
  {MASK,        XK_Right,   "nwin",   NULL},
  {MASK,        XK_Left,    "pwin",   NULL},
  {MASK,        XK_u,       "reset",  NULL}
};

typedef void (*func)();

static void tile_grid();
static void tile_dualstack();
static void tile_fibonacci();
static void tile_bottomstack();

const func modes[] = {
  tile_grid,
  tile_dualstack,
  tile_fibonacci,
  tile_bottomstack
};
