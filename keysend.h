/****************************************************************************
*                                                                           *
*   Sending keycodes or keysyms to x11.                                     *
*                                                                           *
*                                                                           *
*   Author:     Olaf Schulz, 2011                                           *
*                                                                           *
****************************************************************************/


#ifndef __KEY_SEND_H__
#define __KEY_SEND_H__

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
//#include <cstdlib>
#include <unistd.h>

#include <gtk/gtk.h>
#include <gdk/gdk.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
//for keycodesend
#include <X11/extensions/XTest.h>


G_BEGIN_DECLS

typedef struct {
	guint                 keyval;
	GdkModifierType       modifiers;
} KeyMod;

static KeyMod 
getKeyModCodes (GdkWindow *rootwin,
             uint       keyval,
             int       modifiers);

/*static XKeyEvent createKeyEvent(Display *display, Window win,
                           Window winRoot, int press,
                           int keycode, int modifiers);
													 */

int keysend(uint keysym, int modifiers);
int keysend2(uint keysym, uint modsym1, uint modsym2);
int keysend3(uint keysym, uint modsym1, uint modsym2, int press_milliseconds);
int keycodesend(int keycode, int is_down);
int keycodesend2(int keycode, int press_milliseconds);

G_END_DECLS

#endif /* __KEY_SEND_H__ */
