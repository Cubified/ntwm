# ntwm v0.1.0

A tiny, frameless, grid-tiling window manager with multimonitor support.

ntwm was written from scratch using Xlib (although a major TODO is to convert to xcb) and was inspired by dwm, frankenwm, and other minimal tiling window managers.

## Screenshot

![ntwm](https://github.com/Cubified/ntwm/blob/master/ntwm.png)

## Size Comparison

| ntwm (no multimonitor support) | 10kb  |
|--------------------------------|-------|
| ntwm (multimonitor support)    | 15kb  |
| dwm                            | 33kb  |
| 2bwm                           | 36kb  |
| i3                             | 343kb |

### Resource Usage

TODO, although not particularly important

## Compiling, Running, and Debugging

### Prerequisites:

    libX11, libxcb
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

## To-Do

* Convert to xcb
  * Note:  `multihead.h` already uses xcb
* Add general polish
