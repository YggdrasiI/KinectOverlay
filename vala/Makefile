VERSION = 0.1 
VALAC      = valac
BINDIR = bin
OUTPUTFILE = $(BINDIR)/overlay
FILES = main.vala OverlayWindow.vala imagebuttons.vala keybinding-manager.vala overlay-manager.vala LoServer.vala LoClient.vala keysend.c Mouse.vala MouseActions.c
PKGFLAGS = --pkg liblo --pkg posix --pkg x11 --pkg gdk-x11-2.0 --pkg gee-1.0 --pkg keysym --pkg gtk+-2.0 --pkg gmodule-2.0



all: $(BINDIR) overlay
overlay:
	$(VALAC) $(PKGFLAGS) -o $(OUTPUTFILE) $(FILES)



$(BINDIR):
	mkdir -p $(BINDIR)
	ln -s ../overlays bin/overlays

