all: config.h ntwm

CC=gcc

CFLAGS=`pkg-config --cflags x11 xrandr`
LIBS=`pkg-config --libs x11 xrandr`

FLAGS=-Os -pipe -s
DEBUGFLAGS=-Og -Wall -pipe -g

SOURCES=ntwm.c
OUT=ntwm

INSTALLDIR=/usr/local/bin

RM=`which rm`
CP=`which cp`

ntwm:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUT) $(LIBS) $(FLAGS)

debug:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUT) $(LIBS) $(DEBUGFLAGS)

install:
	test -d $(INSTALLDIR) || mkdir -p $(INSTALLDIR)
	install -pm 755 $(OUT) $(INSTALLDIR)

uninstall:
	$(RM) $(INSTALLDIR)/$(OUT)

config.h:
	if [ ! -e "config.h" ]; then $(CP) "config.def.h" "config.h"; fi

clean:
	if [ -e "$(OUT)" ]; then $(RM) $(OUT); fi
