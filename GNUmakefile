.POSIX:
.SUFFIXES:
.PHONY: all clean install check

PROJECT   =c-draw-board
VERSION   =1.0.0
CC        =$(shell which $(TPREFIX)cc $(TPREFIX)gcc 2>/dev/null | head -n 1)
CFLAGS    =-Wall -g3 -std=c99
PREFIX    =/usr/local
BUILDDIR ?=.build
UNAME_S  ?=$(shell uname -s)
EXE      ?=$(shell uname -s | awk '/Windows/ || /MSYS/ || /CYG/ { print ".exe" }')
PROGS     =$(BUILDDIR)/xaoc$(EXE) $(BUILDDIR)/xmaze$(EXE)
DEPS      =`pkg-config --cflags --libs raylib` -lm

all: $(PROGS)
clean:
	rm -f $(PROGS)
install:
	mkdir -p $(DESTDIR)$(PREFIX)/bin
	cp $(BUILDDIR)/xmaze$(EXE) $(DESTDIR)$(PREFIX)/bin
	cp $(BUILDDIR)/xaoc$(EXE) $(DESTDIR)$(PREFIX)/bin
check:
	true

# Programs.
$(BUILDDIR)/xaoc$(EXE): xaoc.c
	$(CC) -o $@ xaoc.c $(CFLAGS) $(CPPFLAGS)
$(BUILDDIR)/xmaze$(EXE): xmaze.c board.c board.h
	$(CC) -o $@ xmaze.c board.c $(CFLAGS) $(CPPFLAGS) $(DEPS)

## -- BLOCK:c --
clean: clean-c
clean-c:
	rm -f *.o
## -- BLOCK:c --
## -- BLOCK:license --
install: install-license
install-license: README.md COPYING COPYRIGHT
	mkdir -p $(DESTDIR)$(PREFIX)/share/doc/$(PROJECT)
	cp README.md COPYING COPYRIGHT $(DESTDIR)$(PREFIX)/share/doc/$(PROJECT)
## -- BLOCK:license --
