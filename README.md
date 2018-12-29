![ntwm](https://github.com/Cubified/ntwm/blob/master/ntwm.png)

# ntwm v1.2.0

A tiny, frameless, keyboard-driven tiling window manager with multimonitor support.

ntwm was written from scratch using Xlib and was inspired by dwm, 2bwm, and other minimal window managers.

## Screenshot

![screenshot](https://github.com/Cubified/ntwm/blob/master/modes/grid.png)

---

## Size Comparison

All of the following window managers have multimonitor support.

| ntwm | 19kb  |
|------|-------|
| dwm  | 33kb  |
| 2bwm | 36kb  |
| i3   | 343kb |

*(Note: these values are approximate and may be subject to change)*

---

## Compiling, Running, Debugging, and Installing

### Prerequisites:

    libX11, libXrandr
    gcc, make

gcc is not necessarily required (tcc or clang can take its place)

### To Compile:

    $ make

### Or:

    $ make debug

---

### To Test (requires Xephyr and xinit):

    $ ./xephyr/run.sh

### Or (requires gdb and xterm in addition to Xephyr and xinit):

    $ ./xephyr/run_debug.sh

---

### To Install:

    $ sudo make install

### To Uninstall:

    $ sudo make uninstall

---

## Using ntwm

### Without a Display Manager

If not using a login/display manager, simply put

    ntwm

into your `~/.xinitrc` and start X using `startx` or `xinit`.

### With a Display Manager (such as LightDM)

First, add an X session by putting this:

    [Desktop Entry]
    Name=ntwm
    Comment=A tiny, frameless, grid-tiling window manager with multimonitor support, written in plain C.
    Exec=ntwm
    TryExec=ntwm
    Type=Application

into `/usr/share/xsessions/ntwm.desktop`.

If your greeter has no option to change the default X session (i.e. _not_ the GTK greeter) edit the `XSession` line in `/var/lib/AccountsService/users/[username]` to read:

    XSession=ntwm

*(Note: these instructions have only been tested with LightDM)*

---

### Default Keybinds

*Note: The default modifier key is Alt*

| Key Combination | Action                             |
|-----------------|------------------------------------|
| Mod+Shift+E     | Exit ntwm                          |
| Mod+Enter       | Start terminal                     |
| Mod+Shift+Q     | Kill focused window                |
| Mod+F           | Toggle gaps                        |
| Mod+Shift+F     | Toggle fullscreen                  |
| Mod+Shift+Right | Move window to next monitor        |
| Mod+Shift+Left  | Move window to previous monitor    |
| Mod+O           | Next tiling mode                   |
| Mod+Right       | Focus next window                  |
| Mod+Left        | Focus previous window              |
| Mod+U           | Reset tiling (for "ghost windows") |

---

## Configuration

All configuration is done through `config.h`, and provided options should be relatively self-explanatory.

### Autostart

ntwm supports running any shell command or script at startup through the `autostartcmd` variable in `config.h`.

### Tiling Modes

By default, ntwm supports the following tiling modes (with more coming):

- Grid

![grid](https://github.com/Cubified/ntwm/blob/master/modes/grid.png)

- Dualstack

![dualstack](https://github.com/Cubified/ntwm/blob/master/modes/dualstack.png)

- Fibonacci

![fibonacci](https://github.com/Cubified/ntwm/blob/master/modes/fibonacci.png)

- Bottomstack

![bottomstack](https://github.com/Cubified/ntwm/blob/master/modes/bottomstack.png)

To edit the order in which these modes are selected, adjust the `modes` variable in `config.h`.

---

## Changelog

### v1.2.0
- Add support for pre-mapped windows being tiled, thereby greatly improving the `reset` command
- Fix signal trapping, replace blocking `XNextEvent` with non-blocking `XCheckMaskEvent`

### v1.1.2

- Fix ghost windows (such as in gimp)
- Fix some dialogs not taking focus (chromium)
- Fix GTK dialogs not centering
- Fix newly-opened GTK windows not tiling correctly
- Add Mod+left/right keybinds to change focus without the mouse
- Add `DIALOG_RATIO` directive in config.h, allow for changes to the size of a dialog on the screen
- Add `-ansi` and `-pedantic` compiler flags, modify code accordingly
- Add `reset` command in config, fix bars being ignored after reset

### v1.1.1

- Add (legitimate) bar support
- Add (partial) support for splash screens and dialogs
- Set `_NET_WM_NAME` atom (meaning neofetch no longer displays ntwm as a de)

### v1.1.0

- Add "nwin" and "pwin" commands in config.h (for cycling through windows)
- Add bottomstack tiling mode
- Add support for changing the tiling mode only on the currently focused monitor
- Add `BAR_HEIGHT` directive in `config.h`
- Add comments
- Move from -Os to -O3 optimization (makes very little difference)
- Remove unnecessary `pkg-config` dependency
- Replace `which` calls with executable paths in Makefile
- Add (impressively mediocre) logo

### v1.0.1

- Add support for switching tiling modes on-the-fly
- Allow for windows to be moved from monitor to monitor (cursor follows)
- Fix toggleable gaps and fullscreen
- Rewrite linked list functions, removing unnecessary loops and fixing infinite loop after opening and closing windows rapidly
- Add support for Xlib atoms, meaning windows closed with Alt+Shift+Q are killed rather than simply unmapped
- Set cursor
- Add dualstack and fibonacci tiling modes
- Add signal trapping
- Add install and uninstall targets in Makefile
- Remove tmux from debug environment
- Add license (GPL v2)

### v1.0.0

- Massively improve stability, especially on multimonitor systems
- Fix invalid pointer error on exit
- Consolidate (and refactor) linked list implementations
- Improve logging utilities
- Add support for changing the Xrandr configuration on-the-fly (screenchange events)
- Remove xcb dependency, replace with a pure-Xlib/Xrandr solution
- Fix xephyr testing/debugging environments

---

## To-Do

- Decrease binary size with -O3 optimization (23k -> 27k, -Os remains at 19k)
- Fix flickering upon dragging chromium tabs

## What Will (Likely) Never be Implemented

- Workspaces
- Full/more complete EWMH/ICCCM support
- Floating mode
