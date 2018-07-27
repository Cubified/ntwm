# ntwm v1.0.1

A tiny, frameless, grid-tiling window manager with multimonitor support.

ntwm was written from scratch using Xlib and was inspired by dwm, 2bwm, and other minimal window managers.

## Screenshot

![ntwm](https://github.com/Cubified/ntwm/blob/master/ntwm.png)

---

## Size Comparison

| ntwm | 15kb  |
|------|-------|
| dwm  | 33kb  |
| 2bwm | 36kb  |
| i3   | 343kb |

---

## Compiling, Running, and Debugging

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

## Configuration

All configuration is done through `config.h`, and provided options should be relatively self-explanatory.

### Autostart

ntwm supports running any shell command or script at startup through the `autostartcmd` variable in `config.h`.

---

## Changelog

### v1.0.1

- Fix toggleable gaps and fullscreen
- Rewrite linked list functions, removing unnecessary loops and fixing infinite loop after opening and closing windows rapidly
- Add support for Xlib atoms, meaning windows closed with Alt+Shift+Q are killed rather than simply unmapped
- Add signal trapping
- Add install and uninstall targets in Makefile
- Remove tmux from debug environment

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

- Set cursor
- Improved/extended atom support
- Bar support
  - Current workaround (for polybar): `override-redirect = true` in bar config
- Borders (?)
  - Would require reparenting (and therefore major refactoring)
