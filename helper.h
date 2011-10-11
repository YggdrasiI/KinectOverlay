/****************************************************************************
*                                                                           *
*   Some helper functions.                                                  *
*                                                                           *
*                                                                           *
*   Author:     Olaf Schulz, 2011                                           *
*                                                                           *
****************************************************************************/

//-----------------------------------------------------------------------------
// Headers
//-----------------------------------------------------------------------------
// General headers
#include <string.h>
#include <stdio.h>
#include <malloc.h>
#include <assert.h>
#include <fcntl.h>
#include <unistd.h>
#include <X11/Xlib.h>

// Gdk (for keysend)
#include <cstdlib>
#include <X11/keysym.h>


/*
Da die Umwandlung des Codes von keysend in cpp 
noch nicht klappte, wird keysend "extern" aufgerufen.
*/
void send_keysym(int keysym, int modifier);

/*
 Auslesen der Mausposition
*/
static int _XlibErrorHandler(Display *display, XErrorEvent *event); 
int getMousePosition(int *root_x, int *root_y, int *win_x, int *win_y); 
