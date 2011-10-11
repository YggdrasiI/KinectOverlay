#include "helper.h"

// +++++++++++++++++++++++++++++++++++++++++++++++++++

/*
Da die Umwandlung des Codes von keysend in cpp 
noch nickt klappte, wird keysend "extern" aufgerufen.
*/
void send_keysym(int keysym, int modifier){

		//char* system_call_str;
		char system_call_str[50];
		int n;
		n = sprintf( system_call_str, "./keysend %i %i", keysym, modifier);
		printf("Call %s\n", system_call_str);
		system(system_call_str);
}

/*
 Auslesen der Mausposition
*/
static int _XlibErrorHandler(Display *display, XErrorEvent *event) {
    fprintf(stderr, "An error occured detecting the mouse position\n");
    return True;
}

int getMousePosition(int *root_x, int *root_y, int *win_x, int *win_y) {
    int number_of_screens;
    int i;
    Bool result;
    Window *root_windows;
    Window window_returned;
    //int root_x, root_y;
    //int win_x, win_y;
    unsigned int mask_return;

    Display *display = XOpenDisplay(NULL);
    assert(display);
    XSetErrorHandler(_XlibErrorHandler);
    number_of_screens = XScreenCount(display);
    fprintf(stderr, "There are %d screens available in this X session\n", number_of_screens);
    root_windows = (Window*) malloc( number_of_screens * sizeof(Window));
    for (i = 0; i < number_of_screens; i++) {
        root_windows[i] = XRootWindow(display, i);
    }
    for (i = 0; i < number_of_screens; i++) {
        result = XQueryPointer(display, root_windows[i], &window_returned,
                //&window_returned, &root_x, &root_y, &win_x, &win_y,
                &window_returned, root_x, root_y, win_x, win_y,
                &mask_return);
        if (result == True) {
            break;
        }
    }
    if (result != True) {
        fprintf(stderr, "No mouse found.\n");
        return -1;
    }
    //printf("Mouse is at (%d,%d)\n", root_x, root_y);

    free(root_windows);
    return 0;
}
