# ntwm v1.0.1

A tiny, frameless, grid-tiling window manager with multimonitor support.

ntwm was written from scratch using Xlib and was inspired by dwm, 2bwm, and other minimal window managers.

## Screenshot

![ntwm](https://github.com/Cubified/ntwm/blob/master/ntwm.png)

---

## Size Comparison

All of the following window managers have multimonitor support.

| ntwm | 15kb  |
|------|-------|
| dwm  | 33kb  |
| 2bwm | 36kb  |
| i3   | 343kb |

*(Note: these values are approximate and may be subject to change)*

---

## Compiling, Running, Debugging, and Installing

### Prerequisites:

    libX11, libXrandr
    gcc, make, pkg-config

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

| Key Combination | Action                          |
|-----------------|---------------------------------|
| Mod+Shift+E     | Exit ntwm                       |
| Mod+Enter       | Start terminal                  |
| Mod+Shift+Q     | Kill focused window             |
| Mod+F           | Toggle gaps                     |
| Mod+Shift+F     | Toggle fullscreen               |
| Mod+Shift+Right | Move window to next monitor     |
| Mod+Shift+Left  | Move window to previous monitor |
| Mod+O           | Next tiling mode                |

---

## Configuration

All configuration is done through `config.h`, and provided options should be relatively self-explanatory.

### Autostart

ntwm supports running any shell command or script at startup through the `autostartcmd` variable in `config.h`.

### Tiling Modes

By default, ntwm supports three tiling modes (with more coming):

- Grid
- Dualstack
- Fibonacci

To edit the order in which these modes are selected, adjust the `modes` variable in `config.h`.

---

## Changelog

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

- More tiling modes
- Per-monitor focused windows (i.e. turning ntwm into a fully keyboard-driven wm)

More long-term, complex, or nonessential plans include:

- Bar support
  - Current workaround (for polybar): `override-redirect = true` in bar config
- Borders (?)
  - Would require reparenting (and therefore major refactoring)
