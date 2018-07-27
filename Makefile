all: config.h ntwm

CC=gcc

CFLAGS=`pkg-config --cflags x11 xrandr`
LIBS=`pkg-config --libs x11 xrandr`

FLAGS=-O2 -pipe -s
DEBUGFLAGS=-Og -Wall -pipe -g

SOURCES=ntwm.c
OUT=ntwm

RM=`which rm`
CP=`which cp`

ntwm:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUT) $(LIBS) $(FLAGS)

debug:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUT) $(LIBS) $(DEBUGFLAGS)

config.h:
	if [ ! -e "config.h" ]; then $(CP) "config.def.h" "config.h"; fi

clean:
	if [ -e "$(OUT)" ]; then $(RM) $(OUT); fi
