all: config.h ntwm

CC=gcc

CFLAGS=-Iinclude -Iconfig
LIBS=-lX11 -lXrandr

FLAGS=-Os -pipe -s -ansi -pedantic
DEBUGFLAGS=-Og -Wall -pipe -g -ansi -pedantic

SOURCES=ntwm.c
OUT=ntwm

INSTALLDIR=/usr/local/bin

RM=/bin/rm
CP=/bin/cp

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
	if [ ! -e "config/config.h" ]; then $(CP) "config/config.def.h" "config/config.h"; fi

clean:
	if [ -e "$(OUT)" ]; then $(RM) $(OUT); fi
