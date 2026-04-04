CC ?= gcc
AR ?= ar
RANLIB ?= ranlib
STRIP ?= strip

CFLAGS += -I.
LDFLAGS += -static

SRC = main.c

shorksay: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o shorksay $(LDFLAGS)
	$(STRIP) shorksay

PREFIX ?= /usr
BINDIR = $(PREFIX)/bin

install: shorksay
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 shorksay $(DESTDIR)$(BINDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/shorksay

clean:
	rm -f shorksay

.PHONY: install uninstall clean
