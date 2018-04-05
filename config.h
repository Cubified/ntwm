/*
 * config.h: ntwm configuration
 */

#define VER "0.1.0"
#define GAPS 50
#define MULTIHEAD /* multimonitor support */
#define MASK Mod1Mask
#define SHIFT ShiftMask

#define TERMCMD "xfce4-terminal"

const bool log_colors = true;
const char *autostartcmd = "true";

typedef struct key {
	unsigned int mod;
	KeySym keysym;
	const char *func;
	const char *arg;
} key;

const key keys[] = {
/*	Mask		Keysym		Command			Argument */
	{MASK|SHIFT,
				XK_e,		"quit",			NULL},
	{MASK,		XK_Return,	"spawn",		TERMCMD},
	{MASK|SHIFT,
				XK_q,		"kill",			NULL},
	{MASK,		XK_f,		"gaps",			NULL}	/* Toggles gaps between windows, entering a "fullscreen" mode */
};
