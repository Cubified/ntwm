# ntwm v1.0.0

A tiny, frameless, grid-tiling window manager with multimonitor support.

ntwm was written from scratch using Xlib and was inspired by dwm, 2bwm, and other minimal tiling window managers.

## Screenshot

![ntwm](https://github.com/Cubified/ntwm/blob/master/ntwm.png)

## Size Comparison

| ntwm                           | 15kb  |
|--------------------------------|-------|
| dwm                            | 33kb  |
| 2bwm                           | 36kb  |
| i3                             | 343kb |

### Resource Usage

TODO, although not particularly important

## Compiling, Running, and Debugging

### Prerequisites:

    libX11, libXrandr
    gcc, make, pkg-config

gcc is not necessarily required (tcc or clang can take its place)

### To compile:

    > make

### Or:

    > make debug

### To test (requires Xephyr and xinit):

    > ./xephyr/run.sh

### Or (requires gdb and xterm in addition to Xephyr and xinit):

    > ./xephyr/run_debug.sh

## Configuration

All configuration is done through `config.h`, and provided options should be relatively self-explanatory.

## What's New in the v1.0.0 Release

- Massively improve stability, especially on multimonitor systems
- Fix invalid pointer error on exit
- Consolidate (and refactor) linked list implementations
- Improve logging utilities
- Add support for changing the Xrandr configuration on-the-fly (screenchange events)
- Remove xcb dependency, replace with a pure-Xlib/Xrandr solution
- Fix xephyr testing/debugging environments

## To-Do

- Allow for gaps to be toggled (/figure out cause of odd crash on monitor creation)
- Bar support
- Borders (?)
  - Would require reparenting (and therefore major refactoring)
