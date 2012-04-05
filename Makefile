OSTYPE := $(shell uname -s)
ARCH := $(shell uname -i)

SRC_FILES = \
	main.cpp \
	signal_catch.cpp \
	kbhit.cpp \
	helper.cpp \
	keysend.cpp

EXE_NAME = KinectOverlay

INC_DIRS += ../../../../../Samples/KinectOverlay

INC_DIRS += $(shell pkg-config --cflags-only-I gtk+-2.0 gdk-2.0 liblo | sed -e 's/-I//' -e 's/ -I/ /g')
#I.E.
#INC_DIRS +=/usr/include/gtk-2.0 /usr/lib/x86_64-linux-gnu/gtk-2.0/include /usr/include/atk-1.0 /usr/include/cairo /usr/include/gdk-pixbuf-2.0 /usr/include/pango-1.0 /usr/include/gio-unix-2.0/ /usr/include/glib-2.0 /usr/lib/x86_64-linux-gnu/glib-2.0/include /usr/include/pixman-1 /usr/include/freetype2 /usr/include/libpng12 

LDFLAGS += $(shell pkg-config --libs x11 xtst liblo gtk+-2.0 gdk-2.0)
#I.E.
#LDFLAGS += -lX11 -lXtst -llo -lpthread -lgtk-x11-2.0 -latk-1.0 -lgio-2.0 -lpangoft2-1.0 -lfreetype -lfontconfig -lgdk-x11-2.0 -lpangocairo-1.0 -lgdk_pixbuf-2.0 -lpango-1.0 -lcairo -lgobject-2.0 -lglib-2.0


# For osc/ osc-cpp wrapper # remove this dependencies...
INC_DIRS += /usr/include/oscpack/ 
LDFLAGS += -L/usr/local/lib -loscpack 

include ../NiteSampleMakefile

