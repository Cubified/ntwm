/*
 * config.h: ntwm configuration
 */

#define VER "1.2.6"
#define GAPS 50
#define STACK_RATIO 0.3
#define DIALOG_RATIO 0.7
#define MULTIHEAD /* multimonitor support */
/*#define LOGGING_NO_STDIO*/ /* avoid including stdio.h for error/info logging (decreases filesize) */
#define MASK Mod1Mask
#define SHIFT ShiftMask

#define MAX_MONITORS 2 /* memory fine-tuning options - ntwm will use no more memory than is specified here */
#define MAX_WINDOWS 16
#define MAX_BARS     2

#define TERMCMD "xterm"

char *autostartcmd = "true";

typedef struct key {
  unsigned int mod;
  KeySym keysym;
  int func;
  char *arg;
} key;

enum {
  cmd_quit,
  cmd_spawn,
  cmd_kill,
  cmd_gaps,
  cmd_full,
  cmd_next,
  cmd_prev,
  cmd_mode,
  cmd_nwin,
  cmd_pwin,
  cmd_reset
};

const key keys[] = {
/* Mask         Keysym      Function    Argument */
  {MASK|SHIFT,  XK_e,       cmd_quit,   NULL},
  {MASK,        XK_Return,  cmd_spawn,  TERMCMD},
  {MASK|SHIFT,  XK_q,       cmd_kill,   NULL},
  {MASK,        XK_f,       cmd_gaps,   NULL},
  {MASK|SHIFT,  XK_f,       cmd_full,   NULL},
  {MASK|SHIFT,  XK_Right,   cmd_next,   NULL},
  {MASK|SHIFT,  XK_Left,    cmd_prev,   NULL},
  {MASK,        XK_o,       cmd_mode,   NULL},
  {MASK,        XK_Right,   cmd_nwin,   NULL},
  {MASK,        XK_Left,    cmd_pwin,   NULL},
  {MASK,        XK_u,       cmd_reset,  NULL}
};

typedef void (*func)();

#define ENABLE_GRID
#define ENABLE_DUALSTACK
#define ENABLE_FIBONACCI
#define ENABLE_BOTTOMSTACK

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
