SRC_FILES = ../KinectOverlay/main.cpp ../KinectOverlay/signal_catch.cpp ../KinectOverlay/kbhit.cpp ../KinectOverlay/helper.cpp ../KinectOverlay/keysend.cpp

EXE_NAME = KinectOverlay_Civ4

ifndef TARGETFS
	TARGETFS=/
endif
 
#LDFLAGS += -B$(TARGETFS)/usr/lib -Wl,--unresolved-symbols=ignore-in-shared-libs
#INC_DIRS += $(PSDK)/Include ../../Include
#LIB_DIRS += $(PSDK)/Platform/Linux-x86/Bin/Release
#BIN_DIR = ../../Bin

#INC_DIRS +=`pkg-config --cflags --libs gtk+-2.0`
INC_DIRS += /usr/include/gtk-2.0 /usr/include/glib-2.0 /usr/include/X11 /usr/include/cairo /usr/include/pango-1.0 /usr/lib/glib-2.0/include /usr/include/gio-unix-2.0 /usr/include
INC_DIRS += /usr/include/gtk-2.0 /usr/lib/gtk-2.0/include /usr/include/atk-1.0 /usr/include/cairo /usr/include/pango-1.0 /usr/include/gio-unix-2.0/ /usr/include/glib-2.0 /usr/lib/glib-2.0/include /usr/include/pixman-1 /usr/include/freetype2 /usr/include/directfb /usr/include/libpng12 
LDFLAGS += -lX11 -lgdk-x11-2.0 -lXtst

# For osc/ osc-cpp wrapper # remove this dependencies...
INC_DIRS += /usr/local/include/oscpack/ 
LDFLAGS += -L/usr/local/lib -loscpack 

# For liblo
INC_DIRS += /usr/include/lo 
LDFLAGS += -llo 

include ../NiteSampleMakefile
