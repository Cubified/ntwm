all: ntwm

# clang is not supported (due to variable-length array declaration)
CC=gcc

CFLAGS=`pkg-config --cflags x11 xcb xcb-randr`	# Remove xcb and xcb-randr if not using multihead
LIBS=`pkg-config --libs x11 xcb xcb-randr`			# See above

FLAGS=-pipe
DEBUGFLAGS=-Og -Wall -pipe -g

SOURCES=ntwm.c
OUT=ntwm

RM=`which rm`

ntwm:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUT) $(LIBS) $(FLAGS)

debug:
	$(CC) $(CFLAGS) $(SOURCES) -o $(OUT) $(LIBS) $(DEBUGFLAGS)

clean:
	if [ -e "$(OUT)" ]; then $(RM) $(OUT); fi
