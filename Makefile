OSTYPE := $(shell uname -s)

SRC_FILES = \
	main.cpp \
	signal_catch.cpp \
	kbhit.cpp \
	helper.cpp \
	keysend.cpp

INC_DIRS += ../../../../../Samples/KinectOverlay

EXE_NAME = KinectOverlay

#INC_DIRS +=`pkg-config --cflags --libs gtk+-2.0`
INC_DIRS += /usr/include/gtk-2.0 /usr/include/glib-2.0 /usr/include/X11 /usr/include/cairo /usr/include/pango-1.0 /usr/lib/glib-2.0/include /usr/include/gio-unix-2.0 /usr/include
INC_DIRS += /usr/include/gtk-2.0 /usr/lib/gtk-2.0/include /usr/include/atk-1.0 /usr/include/cairo /usr/include/pango-1.0 /usr/include/gio-unix-2.0/ /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/include/pixman-1 /usr/include/freetype2 /usr/include/directfb /usr/include/libpng12 

#for u12.4
INC_DIRS += /usr/lib/i386-linux-gnu/glib-2.0/include /usr/lib/i386-linux-gnu/gtk-2.0/include  /usr/include/gdk-pixbuf-2.0

LDFLAGS += -lX11 -lgdk-x11-2.0 -lXtst
# For osc/ osc-cpp wrapper # remove this dependencies...
INC_DIRS += /usr/include/oscpack/ 
LDFLAGS += -L/usr/local/lib -loscpack 

# For liblo
INC_DIRS += /usr/include/lo 
LDFLAGS += -llo 

include ../NiteSampleMakefile

