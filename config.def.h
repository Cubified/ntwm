/*
 * config.h: ntwm configuration
 */

#define VER "1.0.1"
#define GAPS 50
#define GRID_RATIO 0.3
#define MULTIHEAD /* multimonitor support */
#define TILING_MODE tile_grid
#define MASK Mod1Mask
#define SHIFT ShiftMask

#define TERMCMD "xterm"

const char *autostartcmd = "true";

typedef struct key {
  unsigned int mod;
  KeySym keysym;
  const char *func;
  const char *arg;
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
  {MASK,        XK_o,       "mode",   NULL}
};
