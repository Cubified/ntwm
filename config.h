/*
 * config.h: ntwm configuration
 */

#define VER "0.1.0"
#define GAPS 50
#define MULTIHEAD /* multimonitor support */
#define MASK Mod1Mask
#define SHIFT ShiftMask

#define TERMCMD "xterm"

const bool log_colors = true;
const char *autostartcmd = "/home/andrew/.2bwm/autostart.sh";

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
  {MASK,        XK_c,       "spawn",  BROWSERCMD},
  {MASK,        XK_d,       "spawn",  LAUNCHERCMD},
  {MASK|SHIFT,  XK_p,       "spawn",  LOCKCMD},
  {MASK|SHIFT,  XK_q,       "kill",   NULL},
  {MASK,        XK_f,       "gaps",   NULL}
};
