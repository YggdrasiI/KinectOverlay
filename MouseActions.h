/****************************************************************************
*                                                                           *
*   Collection of wrapper functions for uinput.                             *
*                                                                           *
*                                                                           *
*   Author:     Olaf Schulz, 2011                                           *
*                                                                           *
****************************************************************************/


#ifndef MOUSE_ACTIONS_H
#define MOUSE_ACTIONS_H

#include <linux/uinput.h>

static int uinp_fd = -1; 
struct uinput_user_dev uinp; // uInput device structure 
struct input_event event; // Input device structure 

// run on startup 
static int setup_uinput_device() 
{ 
    int i=0; 
    uinp_fd = open("/dev/uinput", O_WRONLY | O_NDELAY); 
    if (!uinp_fd) 
    { 
        printf("Unable to open /dev/uinput\n"); 
        return -1; 
    } 
    memset(&uinp,0,sizeof(uinp)); // Intialize the uInput device to NULL 
    strncpy(uinp.name, "Kinect Mouse", UINPUT_MAX_NAME_SIZE); 
    uinp.id.version = 4; 
    uinp.id.bustype = BUS_USB; 
    ioctl(uinp_fd, UI_SET_EVBIT, EV_KEY); 
    ioctl(uinp_fd, UI_SET_EVBIT, EV_REL); 
    ioctl(uinp_fd, UI_SET_RELBIT, REL_X); 
    ioctl(uinp_fd, UI_SET_RELBIT, REL_Y); 

		//for absolute movements
		/*
		ioctl(uinp_fd, UI_SET_EVBIT, EV_ABS);
		ioctl(uinp_fd, UI_SET_ABSBIT, ABS_X);
		ioctl(uinp_fd, UI_SET_ABSBIT, ABS_Y);
		uinp.absmin[ABS_X] = 0;
		uinp.absmax[ABS_X] = 1023;
		uinp.absmin[ABS_Y] = 0;
		uinp.absmax[ABS_Y] = 719;
		*/

    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE); 
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_TOUCH); 
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MOUSE);//redundant?
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_LEFT); 
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_MIDDLE); 
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_RIGHT); 
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_FORWARD); 
    ioctl(uinp_fd, UI_SET_KEYBIT, BTN_BACK); 
    /* Create input device into input sub-system */ 
    write(uinp_fd, &uinp, sizeof(uinp)); 
    if (ioctl(uinp_fd, UI_DEV_CREATE)) 
    { 
        printf("Unable to create UINPUT device."); 
        return -1; 
    } 
    return 1; 
} 
	
	static void send_mouse_down(int button)
	{
					// Report BUTTON CLICK - PRESS event
					memset(&event, 0, sizeof(event));
					gettimeofday(&event.time, NULL);
					event.type = EV_KEY;
					event.code = button;//BTN_LEFT;
					event.value = 1;
					write(uinp_fd, &event, sizeof(event));
					event.type = EV_SYN;
					event.code = SYN_REPORT;
					event.value = 0;
					write(uinp_fd, &event, sizeof(event));
	}


	static void send_mouse_up(int button)
	{
					// Report BUTTON CLICK - RELEASE event
					memset(&event, 0, sizeof(event));
					gettimeofday(&event.time, NULL);
					event.type = EV_KEY;
					event.code = button;//BTN_LEFT;
					event.value = 0;
					write(uinp_fd, &event, sizeof(event));
					event.type = EV_SYN;
					event.code = SYN_REPORT;
					event.value = 0;
					write(uinp_fd, &event, sizeof(event));
	}

static void move_cursor(int x, int y ) 
{ 
    memset(&event, 0, sizeof(event)); 
    gettimeofday(&event.time, NULL); 
    event.type = EV_REL; 
    event.code = REL_X; 
    event.value = x; 
    write(uinp_fd, &event, sizeof(event)); 
    event.type = EV_REL; 
    event.code = REL_Y; 
    event.value = y; 
    write(uinp_fd, &event, sizeof(event)); 
    event.type = EV_SYN; 
    event.code = SYN_REPORT; 
    event.value = 0; 
    write(uinp_fd, &event, sizeof(event)); 
}

static void move_cursor_to(int x, int y ) 
{ 
    memset(&event, 0, sizeof(event)); 
    gettimeofday(&event.time, NULL); 
    event.type = EV_ABS; 
    event.code = ABS_X; 
    event.value = x; 
    write(uinp_fd, &event, sizeof(event)); 
    event.type = EV_ABS; 
    event.code = ABS_Y; 
    event.value = y; 
    write(uinp_fd, &event, sizeof(event)); 
    event.type = EV_SYN; 
    event.code = SYN_REPORT; 
    event.value = 0; 
    write(uinp_fd, &event, sizeof(event)); 
}

/*
See linux/input.h for keycodes
*/
void send_keycode(int KEYCODE)
{
// Report BUTTON CLICK - PRESS event
memset(&event, 0, sizeof(event));
gettimeofday(&event.time, NULL);
event.type = EV_KEY;
event.code = KEYCODE;
event.value = 1;
write(uinp_fd, &event, sizeof(event));
event.type = EV_SYN;
event.code = SYN_REPORT;
event.value = 0;
write(uinp_fd, &event, sizeof(event));
// Report BUTTON CLICK - RELEASE event
memset(&event, 0, sizeof(event));
gettimeofday(&event.time, NULL);
event.type = EV_KEY;
event.code = KEYCODE;
event.value = 0;
write(uinp_fd, &event, sizeof(event));
}




	static void XN_CALLBACK_TYPE Push_Left_Mouse_Button(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		send_mouse_down(BTN_LEFT);
		send_mouse_up(BTN_LEFT);
	}
	
	static void XN_CALLBACK_TYPE Push_Right_Mouse_Button(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		send_mouse_down(BTN_RIGHT);
		send_mouse_up(BTN_RIGHT);
	}


// --------------------------------------------------------------------------
// CALLBACKS 
// ++ For LoServer event


#endif
