/****************************************************************************
 *                                                                           *
 *   Sending keycodes or keysyms to x11.                                     *
 *                                                                           *
 *                                                                           *
 *   Author:     Olaf Schulz, 2011                                           *
 *                                                                           *
 ****************************************************************************/

/* keysend.c
 * Copyright (C) 2008 Alex Graveley
 * Copyright (C) 2010 Ulrik Sverdrup <ulrik.sverdrup@gmail.com>
 * Copyright (C) 2011 Olaf Schulz  <schulz[AT]math[DOT]hu-berlin[DOT]de> (modified bind.c)
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE
 * LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION
 * OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 */
#include "keysend.h"

/* Uncomment the next line to print a debug trace. */
/* #define DEBUG */

#ifdef DEBUG
#  define TRACE(x) x
#else
#  define TRACE(x) do {} while (FALSE);
#endif

#define MODIFIERS_ERROR ((GdkModifierType)(-1))
#define MODIFIERS_NONE 0

/* Group to use: Which of configured keyboard Layouts
 * Since grabbing a key blocks its use, we can't grab the corresponding
 * (physical) keys for alternative layouts.
 *
 * Because of this, we interpret all keys relative to the default
 * keyboard layout.
 *
 * For example, if you bind "w", the physical W key will respond to
 * the bound key, even if you switch to a keyboard layout where the W key
 * types a different letter.
 */
#define WE_ONLY_USE_ONE_GROUP 0

// The key code to be sent.
// A full list of available codes can be found in /usr/include/X11/keysymdef.h
//#define KEYCODE XK_Down
//#define KEYCODE XK_a


/* Return the modifier mask that needs to be pressed to produce key in the
 * given group (keyboard layout) and level ("shift level").
 */
	static GdkModifierType
FinallyGetModifiersForKeycode (XkbDescPtr xkb,
		KeyCode    key,
		uint     group,
		uint     level)
{
	int nKeyGroups;
	int effectiveGroup;
	XkbKeyTypeRec *type;
	int k;

	nKeyGroups = XkbKeyNumGroups(xkb, key);
	if ((!XkbKeycodeInRange(xkb, key)) || (nKeyGroups == 0)) {
		return MODIFIERS_ERROR;
	}

	/* Taken from GDK's MyEnhancedXkbTranslateKeyCode */
	/* find the offset of the effective group */
	effectiveGroup = group;
	if (effectiveGroup >= nKeyGroups) {
		unsigned groupInfo = XkbKeyGroupInfo(xkb,key);
		switch (XkbOutOfRangeGroupAction(groupInfo)) {
			default:
				effectiveGroup %= nKeyGroups;
				break;
			case XkbClampIntoRange:
				effectiveGroup = nKeyGroups-1;
				break;
			case XkbRedirectIntoRange:
				effectiveGroup = XkbOutOfRangeGroupNumber(groupInfo);
				if (effectiveGroup >= nKeyGroups)
					effectiveGroup = 0;
				break;
		}
	}
	type = XkbKeyKeyType(xkb, key, effectiveGroup);
	for (k = 0; k < type->map_count; k++) {
		if (type->map[k].active && type->map[k].level == level) {
			if (type->preserve) {
				return (GdkModifierType) (type->map[k].mods.mask &
						~type->preserve[k].mask);
			} else {
				return (GdkModifierType) (type->map[k].mods.mask);
			}
		}
	}
	return (GdkModifierType) MODIFIERS_NONE;
}

	static KeyMod 
getKeyModCodes (GdkWindow *rootwin,
		uint       keyval,
		int       modifiers)
{
	int k;
	GdkKeymap *map;
	GdkKeymapKey *keys;
	gint n_keys;
	GdkModifierType add_modifiers;
	XkbDescPtr xmap;
	//	gboolean success = FALSE;
	KeyMod keymod;

	xmap = XkbGetMap(GDK_WINDOW_XDISPLAY(rootwin),
			XkbAllClientInfoMask,
			XkbUseCoreKbd);

	map = gdk_keymap_get_default();
	gdk_keymap_get_entries_for_keyval(map, keyval, &keys, &n_keys);

	if (n_keys == 0){
		//		return FALSE;
		keymod.keyval = -1;
		keymod.modifiers = (GdkModifierType)0;
		return keymod;
	}

	for (k = 0; k < n_keys; k++) {
		/* NOTE: We only bind for the first group,
		 * so regardless of current keyboard layout, it will
		 * grab the key from the default Layout.
		 */
		if (keys[k].group != WE_ONLY_USE_ONE_GROUP) {
			continue;
		}

		add_modifiers = FinallyGetModifiersForKeycode(xmap,
				keys[k].keycode,
				keys[k].group,
				keys[k].level);

		if (add_modifiers == MODIFIERS_ERROR) {
			continue;
		}
		//		TRACE (g_print("grab/ungrab keycode: %d, lev: %d, grp: %d, ", keys[k].keycode, keys[k].level, keys[k].group));
		//		TRACE (g_print("modifiers: 0x%x (consumed: 0x%x)\n", add_modifiers | modifiers, add_modifiers));
		// zu wählen ist 'add_modifiers | modifiers'
		keymod.keyval = keys[k].keycode;
		keymod.modifiers = (GdkModifierType) (add_modifiers | modifiers);
	}

	g_free(keys);
	XkbFreeClientMap(xmap, 0, True);

	return keymod;
	//return success;
}


// Function to create a keyboard event
static XKeyEvent createKeyEvent(Display *display, Window win,
		Window winRoot, int press,
		int keycode, int modifiers)
{
	XKeyEvent event;

	event.display     = display;
	event.window      = win;
	event.root        = winRoot;
	event.subwindow   = None;
	event.time        = CurrentTime;
	event.x           = 1;
	event.y           = 1;
	event.x_root      = 1;
	event.y_root      = 1;
	event.same_screen = True;
	event.keycode     = keycode;
	event.state       = modifiers;

	if(press)
		event.type = KeyPress;
	else
		event.type = KeyRelease;

	//test, bringen keine Verschlechterung...
	//event.send_event=true;
	//event.serial = 0;

	return event;
}

int
keysend(uint keysym, int modifiers){

	//int keysym;
	KeyMod keymod;

	// Obtain the X11 display.
	Display *display = XOpenDisplay(0);
	if(display == NULL)
		return -1;

	//von bind.c
	//  gdk_init (&argc, &argv);//call this in main routine
	GdkWindow *rootwin = gdk_get_default_root_window ();

	// Get the root window for the current display.
	Window winRoot = XDefaultRootWindow(display);

	// Find the window which has the current keyboard focus.
	Window winFocus;
	int    revert;
	XGetInputFocus(display, &winFocus, &revert);

	keymod = getKeyModCodes(rootwin, keysym, modifiers);

	// Send a fake key press event to the window.
	XKeyEvent event = createKeyEvent(display, winFocus, winRoot, True, keymod.keyval, keymod.modifiers);
	XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

	// Send a fake key release event to the window.
	event = createKeyEvent(display, winFocus, winRoot, False, keymod.keyval, keymod.modifiers);
	XSendEvent(event.display, event.window, True, KeyReleaseMask, (XEvent *)&event);

	// Done.
	XCloseDisplay(display);
	return 0;
}


int keysend2(uint keysym, uint modsym1, uint modsym2) {
	return keysend3(keysym, modsym1, modsym2, 0);
}

int keysend3(uint keysym, uint modsym1, uint modsym2, int press_milliseconds) {

	KeyMod keymod;

	// Obtain the X11 display.
	Display *display = XOpenDisplay(0);
	if(display == NULL)
		return -1;

	//von bind.c
	//  gdk_init (&argc, &argv);//call this in main routine
	GdkWindow *rootwin = gdk_get_default_root_window ();

	// Get the root window for the current display.
	Window winRoot = XDefaultRootWindow(display);

	// Find the window which has the current keyboard focus.
	Window winFocus;
	int    revert;
	XGetInputFocus(display, &winFocus, &revert);

	int xkeycode = -1;
	int xmodcode1 = -1;
	int xmodcode2 = -1;
	XKeyEvent event;

	xkeycode = XKeysymToKeycode(display, keysym);

	if( modsym2 != 0 ){
		xmodcode2 = XKeysymToKeycode(display, modsym2);
		event = createKeyEvent(display, winFocus, winRoot, True, xmodcode2, 0);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
	}
	if( modsym1 != 0 ){
		xmodcode1 = XKeysymToKeycode(display, modsym1);
		event = createKeyEvent(display, winFocus, winRoot, True, xmodcode1, 0);
		XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);
	}

	// Send a fake key press event to the window.
	event = createKeyEvent(display, winFocus, winRoot, True, xkeycode, 0);
	XSendEvent(event.display, event.window, True, KeyPressMask, (XEvent *)&event);

	//wait
	usleep(press_milliseconds * 1000);

	// Send a fake key release event to the window.
	event = createKeyEvent(display, winFocus, winRoot, False, xkeycode, 0);
	XSendEvent(event.display, event.window, True, KeyReleaseMask, (XEvent *)&event);

	if( modsym1 != 0 ){
		event = createKeyEvent(display, winFocus, winRoot, False, xmodcode1, 0);
		XSendEvent(event.display, event.window, True, KeyReleaseMask, (XEvent *)&event);
	}
	if( modsym2 != 0 ){
		event = createKeyEvent(display, winFocus, winRoot, False, xmodcode2, 0);
		XSendEvent(event.display, event.window, True, KeyReleaseMask, (XEvent *)&event);
	}

	// Done.
	XCloseDisplay(display);
	return 0;
}

/* Simulate Keystrokes, code from xsendkeycode.h
	 Input are the keycodes, not keysyms!
 */
int keycodesend(int keycode, int is_down)
{
	Display *dpy;
	//int scr;
	//Window win;
	//unsigned int width, height;
	//Window ret_win;
	//int x, y;
	//unsigned int border_width, depth;

	dpy = XOpenDisplay("");
	if (!dpy) {
		fprintf(stderr, "%s: Cannot connect to display ...\n");
		return 1;
	}
	//scr = DefaultScreen(dpy);
	//win = RootWindow(dpy, scr);
	//XGetGeometry(dpy, win, &ret_win, &x, &y, &width, &height, &border_width, &depth);
	//fprintf(stderr,"%dx%d keycode %d is_down %d\n", width, height, keycode, is_down);
	XTestFakeKeyEvent(dpy, keycode, is_down, 0);
	XSync(dpy, 1);
	XCloseDisplay(dpy);
	return 0;
}

/*
	 Send up and down event.
 */
int keycodesend2(int keycode, int press_milliseconds){
	//key down	
	keycodesend(keycode, 1);

	//wait
	usleep(press_milliseconds * 1000);

	//key up
	keycodesend(keycode, 0);

	return 0;
}

