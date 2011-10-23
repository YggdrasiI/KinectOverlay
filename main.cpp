/****************************************************************************
 *                                                                           *
 *   Overlay Example.                                                        *
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
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <linux/input.h>
#include <linux/uinput.h>
#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <math.h>

// Gdk (for keysend)
#include <cstdlib>
#include <X11/keysym.h>
#include <gtk/gtk.h>
#include <gdk/gdk.h>
//#include <X11/Xlib.h>
/*
#include <X11/Xlib.h>
#include <X11/XKBlib.h>
#include <X11/keysym.h>
 */

// OpenNI headers
#include <XnOpenNI.h>
// NITE headers
#include <XnVSessionManager.h>
#include "XnVMultiProcessFlowClient.h"
#include <XnVWaveDetector.h>
#include <XnVPushDetector.h>
#include <XnVPointFilter.h>
#include <XnVSwipeDetector.h>
#include <XnVSteadyDetector.h>
#include <XnVBroadcaster.h>
#include <XnVFlowRouter.h>

#include <X11/Xlib.h>

//local headers
#include "kbhit.h"
#include "signal_catch.h"
#include "helper.h"
#include "XnVSecondaryFilter.h"
#include "MyBox2.h"
#include "MySlider2D.h"
#include "MyCircleDetectorWithPush.h"
#include "MyPointDetector.h"
// for sending ocs events
#include "OverlaySend.h"

// for reciving events
#include "LoServer.h"





// xml to initialize OpenNI
#define SAMPLE_XML_FILE "../../Data/Sample-Tracking.xml"

static int display_width;
static int display_height;
static const int DOUBLE_PUSH_TIME = 10000;

XnBool g_bQuit = false;

// global(?) variables
XnVFlowRouter* g_pMainFlowRouter;
XnVFlowRouter* g_pSecondFlowRouter;
MyBox2* g_pMyBox2;
MySlider2D* g_pMySlider;
MyPointDetector* g_pPointDetector;
XnVPushDetector* g_pDummy;
//XnVCircleDetector* g_pCircle;
MyCircleDetectorWithPush* g_pCircle;
XnVMessageListener* g_pVoidListener;
int g_circle_count;
bool g_circle_start = FALSE;
char g_active_overlay[40];
XnUInt64 g_nPrimaryPushTimestamp = 0;
XnUInt64 g_nSecondaryPushTimestamp = 0;
bool g_bprimaryPushActive = false;
bool g_bsecondaryPushActive = false;
bool g_bRightMouseButtonPressed = false;


static XnVPushDetector pd1;//now static/global
static XnVSecondaryFilter sf;
static LoServer loserver; //(with own thread)
//static XnPoint3D g_currentPrimaryPosition;
//static XnPoint3D g_currentSecondaryPosition;


// Press space bar with uinput
// Do not work...
void send_space_button()
{
	int root_x, root_y;
	int win_x, win_y;
	int ret;
	ret = getMousePosition(&root_x, &root_y, &win_x, &win_y);

	//printf("Send Space, Cursorposition: %i %i\n Relativ zum aktiven Fenster: : %i %i\n", root_x, root_y, win_x, win_y);
	if( ret < 0) return;

	if( root_y < 0.75*display_height)
		send_keysym(XK_b,0);
	//send_keysym(XK_space,0);
	else
		send_keysym(XK_space,0);

}

void show_overlay(char* uiname){
	// <Ctrl><Alt>v
	//	send_keysym(XK_v,12);
	//char uiname[] = "Civ4.ui";
	// char uiname[] = "";
	//	civ4_change_overlay(uiname,2);
	civ4_change_overlay(uiname,1);
	overlay_active = TRUE;
}

void hide_overlay(){
	char uiname[] = "";
	civ4_change_overlay(uiname,0);
	//Rest done in LoServer feedback
	/*overlay_active = FALSE;
		mouse_move_active = TRUE;
		g_pMainFlowRouter->SetActive( &pd1 );*/
}



//-----------------------------------------------------------------------------
// Callbacks
//-----------------------------------------------------------------------------

// Callback for when the focus is in progress
void XN_CALLBACK_TYPE SessionProgress(const XnChar* strFocus, const XnPoint3D& ptFocusPoint, XnFloat fProgress, void* UserCxt)
{
	printf("Session progress (%6.2f,%6.2f,%6.2f) - %6.2f [%s]\n", ptFocusPoint.X, ptFocusPoint.Y, ptFocusPoint.Z, fProgress,  strFocus);
}
// callback for session start
void XN_CALLBACK_TYPE SessionStart(const XnPoint3D& ptFocusPoint, void* UserCxt)
{
	printf("Session started. Please wave (%6.2f,%6.2f,%6.2f)...\n", ptFocusPoint.X, ptFocusPoint.Y, ptFocusPoint.Z);
}
// Callback for session end
void XN_CALLBACK_TYPE SessionEnd(void* UserCxt)
{
	printf("Session ended. Please perform focus gesture to start session\n");
}
// Callback for wave detection
void XN_CALLBACK_TYPE OnWaveCB(void* cxt)
{
	printf("Wave!\n");
	//send_click_left();
}

// callback for a new position of any hand
/*
	 void XN_CALLBACK_TYPE OnPointUpdate(const XnVHandPointContext* pContext, void* cxt)
	 {
//printf("%d: (%f,%f,%f) [%f]\n", pContext->nID, pContext->ptPosition.X, pContext->ptPosition.Y, pContext->ptPosition.Z, pContext->fTime);
//printf("%f,%f,%f\n",  pContext->ptPosition.X, pContext->ptPosition.Y, pContext->ptPosition.Z);

XnUInt32 nContext = pContext->nID;

if( nContext != sf.nPrimary ){
g_currentSecondaryPosition.X = pContext->ptPosition.X;
g_currentSecondaryPosition.Y = pContext->ptPosition.Y;
g_currentSecondaryPosition.Z = pContext->ptPosition.Z;
return;
}
g_currentPrimaryPosition.X = pContext->ptPosition.X;
g_currentPrimaryPosition.Y = pContext->ptPosition.Y;
g_currentPrimaryPosition.Z = pContext->ptPosition.Z;

position[0] = position[0] - weight2*positionArr[posI][0];
positionArr[posI][0] = pContext->ptPosition.X*4;
position[0] *= factor;
position[0] += weight1*positionArr[posI][0];

position[1] = position[1] - weight2*positionArr[posI][1];
positionArr[posI][1] = pContext->ptPosition.Y*2;
position[1] *= factor;
position[1] += weight1*positionArr[posI][1];

currentAbsoluteX = (int)(position[0]);
currentAbsoluteY = (int)(position[1]);
if( !mouse_move_active ) return;
move_cursor(currentAbsoluteX-lastAbsoluteX, lastAbsoluteY-currentAbsoluteY);
lastAbsoluteX = currentAbsoluteX;
lastAbsoluteY = currentAbsoluteY; 

posI = (posI+1) % MAXCOORDS;
}*/

// Callback for Push/Stable
// Show Overlay
void XN_CALLBACK_TYPE PrimaryOnPushCB(XnFloat fVelocity, XnFloat fAngle, void* cxt)
{
	//update timestamp
	xnOSGetHighResTimeStamp(&g_nPrimaryPushTimestamp);
	g_bprimaryPushActive = true;

	//Wait an small amount of time for other events. This increase the reaction time, but leads to better
	// detection of double push events. //ok, bad idea, both events are handled in the same thread...
	//usleep( DOUBLE_PUSH_TIME ); 


	if( ! g_bprimaryPushActive ) return;

	if( overlay_active == FALSE ){
		//check, if push with both hands
		if(false &&  g_nPrimaryPushTimestamp - g_nSecondaryPushTimestamp < DOUBLE_PUSH_TIME ){
			printf("Double Push!! Diff was %i\n", g_nPrimaryPushTimestamp - g_nSecondaryPushTimestamp );
			if( g_pPointDetector->getMode() != 3){
				g_pPointDetector->setMode(3);
			}else{
				g_pPointDetector->setMode(1);
			}
		}else if( !scale_active ){
			printf("'Activate overlay' Push\n");

			//Select overlayname. 
			if( //strcmp( g_active_overlay, "") == 0 ||
					strcmp( g_active_overlay, "Civ4Numpad.ui") == 0 ||
					//strcmp( g_active_overlay, "Civ4Citymenu.ui") == 0 ||
					//strncmp( g_active_overlay, "Civ4Menu", 8 ) == 0 )
				strncmp( g_active_overlay, "Civ4Menu2", 9 ) == 0
					){
						char uiname[] = "defaultOverlay";
						civ4_change_overlay(uiname,1);
					}else{
						char uiname[] = "";//select last active
						civ4_change_overlay(uiname,1);
					}

			//g_pMySlider->Reposition(g_currentPrimaryPosition);
			g_pMySlider->Reposition( g_pPointDetector->m_psmoothPrimaryPositionLongtime->getSmooth() );

			/* //The rest of should be done in the loServer feedback function.
				 overlay_active = TRUE;
				 mouse_move_active = FALSE;
			//g_pMySlider->Reposition(g_currentPrimaryPosition);
			g_pMainFlowRouter->SetActive(g_pMySlider);
			 */
		}else{
			//Disable scale mode
			g_pPointDetector->setMode(1);
		}
	}else{
		hide_overlay();
		//or
		/*
		//send click to lo-Server
		printf("'Click in overlay' PushDetector 1\n");
		MySlider2D* mySlider = g_pMySlider; //(MySlider2D*)cxt;
		push_cell((int) ( mySlider->GetItemYCount()-1-mySlider->m_YIndex), (int) mySlider->m_XIndex );
		 */
	}
}

void XN_CALLBACK_TYPE PrimaryOnStableCB(XnFloat fVelocity, void* cxt)
{
	g_bprimaryPushActive = false;
	printf("Primary Stable\n" );
	fflush(stdout);
}

// Callback Functions for second Hand
// PushDetector callback function for inactive MyBox2
/*
	 void XN_CALLBACK_TYPE OnPushCB2(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	 {
	 XnUInt64 nNow;
	 XnStatus res = xnOSGetHighResTimeStamp(&nNow); //Printing also the timestamp of the Push
/*
if(XN_STATUS_OK == res)
{
printf("%d: %s: Push2\n", XnUInt32(nNow), (XnChar*)cxt);
}
else
{
printf("%s: Push2\n", (XnChar*)cxt);
}* /

// Switch Active Element
printf("Push2, activate myMouseBox\n");
g_pSecondFlowRouter->SetActive(g_pMyBox2);
}

void XN_CALLBACK_TYPE OnStableCB2(XnFloat fVelocity, void* cxt)
{
XnUInt64 nNow;
XnStatus res = xnOSGetHighResTimeStamp(&nNow); //Printing also the timestamp of the Stable
if(XN_STATUS_OK == res)
{
printf("%d: %s: Stable\n", XnUInt32(nNow), (XnChar*)cxt);
}
else
{
printf("%s: Stable\n", (XnChar*)cxt);
}

}
 */


// Push detector for active MyBox2
void XN_CALLBACK_TYPE SecondaryOnPushCB(XnFloat fVelocity, XnFloat fAngle, void* cxt)
{
	//update timestamp
	xnOSGetHighResTimeStamp(&g_nSecondaryPushTimestamp);

	//Wait an small amount of time for other events. This increase the reaction time, but leads to better
	// detection of double push events. //ok, bad idea, both events are handled in the same thread...
	//usleep( DOUBLE_PUSH_TIME ); 

	MyBox2* box = (MyBox2*)(cxt);
	// Leave the box
	//box->Leave();
	printf("Secondary Push, %i\n", g_nSecondaryPushTimestamp);
	if( overlay_active == TRUE && circle_active == FALSE && mouse_move_active == FALSE ){
		printf("Secondary Push, push cell\n");
		MySlider2D* mySlider = g_pMySlider; //(MySlider2D*)cxt;
		push_cell( (int) ( mySlider->GetItemYCount()-1-mySlider->m_YIndex), (int) mySlider->m_XIndex );
	}else{

		//check, if push with both hands
		if(false && g_nSecondaryPushTimestamp-g_nPrimaryPushTimestamp<DOUBLE_PUSH_TIME ){
			printf("Double Push!! Diff was %i\n", g_nSecondaryPushTimestamp-g_nPrimaryPushTimestamp);
			if( g_pPointDetector->getMode() != 3){
				g_pPointDetector->setMode(3);
			}else{
				g_pPointDetector->setMode(1);
			}
		}else if( !scale_active ){ // push only, if not in scale mode
			printf("Secondary Push\n");
			/*   int root_x, root_y;
					 int win_x, win_y;
					 int ret;
					 ret = getMousePosition(&root_x, &root_y, &win_x, &win_y);

					 if( ret < 0) return;

					 if( root_y < 0.75*display_height)
					 send_mouse_down(BTN_LEFT);
					 send_mouse_up(BTN_LEFT);
					 send_mouse_up(BTN_LEFT);
					 else{
					 send_mouse_down(BTN_RIGHT);
					 send_mouse_up(BTN_RIGHT);
					 }
			 */
			if( g_pPointDetector->m_secondaryPosition.Y < 400){

				if( g_bRightMouseButtonPressed ){
					g_bRightMouseButtonPressed = false;
					g_pPointDetector->m_bAllowOnlyEightDirections = false;
					//m_psmoothPrimaryPositionLongtime->m_bactive = true;
					send_mouse_up(BTN_RIGHT);
				}else{
					send_mouse_down(BTN_LEFT);
					send_mouse_up(BTN_LEFT);
				}
			}/*else{
				 send_mouse_down(BTN_RIGHT);
				 send_mouse_up(BTN_RIGHT);
				 }*/
			else{

				if( g_bRightMouseButtonPressed ){
					//release right mouse button
					g_bRightMouseButtonPressed = false;
					g_pPointDetector->m_bAllowOnlyEightDirections = false;
					//m_psmoothPrimaryPositionLongtime->m_bactive = true;
					send_mouse_up(BTN_RIGHT);
				}else{
					g_bRightMouseButtonPressed = true;
					g_pPointDetector->m_bAllowOnlyEightDirections = true;
					//m_psmoothPrimaryPositionLongtime->m_bactive = false;
					send_mouse_down(BTN_RIGHT);
				}
			}
		}else{
			//Disable scale mode
			//g_pPointDetector->setMode(1);
		}
	}

}

void XN_CALLBACK_TYPE SecondaryOnStableCB(XnFloat fVelocity, void* cxt){

}

// For MySlider2D
static void XN_CALLBACK_TYPE TrackPad_ItemSelect(XnInt32 nXItem, XnInt32 nYItem, XnVDirection eDir, void* cxt)
{
	printf("Select: %d,%d (%s)\n", nXItem, nYItem, XnVDirectionAsString(eDir));

	//hide_overlay();//do noting here, see primary push detector (pd1)

	//MySlider2D* pMySlider = ((MySlider2D*)cxt);
	//pMySlider->m_pFlowRouter->SetActive(  pMySlider->m_pPd  );
	//	g_pMainFlowRouter->SetActive( &pd1 );

	//Set input center //no Point available...
	//g_pMyMouseBox->SetSwipeActive();
	//Deactivate
	//((MySlider2D*)cxt)->Activate(false);
}

static void XN_CALLBACK_TYPE TrackPad_PrimaryDestroy(XnUInt32 nID, void* UserCxt)
{
	printf("TrackPad input has stopped!!!\n");
	//hide_overlay();
}

void XN_CALLBACK_TYPE CircleCB(XnFloat fTimes, XnBool bConfident, const XnVCircle* pCircle, void* pUserCxt)
{ 
	//SetCircle(true, fmod((double)fTimes, 1.0) * 2 * XnVMathCommon::PI);
	//printf("CircleCB, %f!\n", fTimes);
	double circle_count2 = fTimes*1.2;
	//double circle_count2 = fTimes;

	if( g_circle_start == FALSE && fabs(fTimes) < 1 ){
		g_circle_count = 0;
		return; 
	}
	g_circle_start = TRUE;

	if ( circle_count2 - g_circle_count >= 1 ){
		g_circle_count = (int) floor(circle_count2);
		if( strcmp(g_active_overlay,"Civ4Citymenu.ui") == 0 ){
			char next[] = "Civ4_NextCity";
			push_named_cell(next);
			printf("CircleCB, (cityscreen) %f!\n", fTimes);
		}else{
			char next[] = "Civ4_CircleNext";
			push_named_cell(next);
			printf("CircleCB, %f!\n", fTimes);
		}
		return;
	}

	if ( g_circle_count - circle_count2 >= 1 ){
		g_circle_count = (int) ceil(circle_count2);
		if( strcmp(g_active_overlay,"Civ4Citymenu.ui") == 0 ){
			char prev[] = "Civ4_PreviousCity";
			push_named_cell(prev);
			printf("CircleCB (cityscreen, counterclockwise), %f!\n", fTimes);
		}else{
			char prev[] = "Civ4_CirclePrevious";
			push_named_cell(prev);
			printf("CircleCB (counterclockwise), %f!\n", fTimes);
		}
		return;
	}

}

void XN_CALLBACK_TYPE NoCircleCB(XnFloat fLastValue, XnVCircleDetector::XnVNoCircleReason reason, void * pUserCxt)
{ 
	printf("No CircleCB!\n");
	g_circle_start = FALSE;
	//g_circle_count = 0;
}

/*
	 void XN_CALLBACK_TYPE Circle_PrimaryCreate(const XnVHandPointContext *cxt, const XnPoint3D& ptFocus, void * pUserCxt)
	 { 
	 }                                                                                                      

	 void XN_CALLBACK_TYPE Circle_PrimaryDestroy(XnUInt32 nID, void * pUserCxt)
	 {
	 }*/

void getResolution(int *display_width, int *display_height){
	Display *disp;
	int screen_num;

	/* connect to the XServer */
	if ((disp = XOpenDisplay(NULL)) == NULL) {
		printf("ERROR - Failed to call XOpenDisplay()\n");
		exit(-1);
	}   

	/* Query the display structure for information */
	screen_num = DefaultScreen(disp);
	*display_width = (int) DisplayWidth(disp, screen_num); // unsigned int
	*display_height = (int) DisplayHeight(disp, screen_num);

	/* Close the Window and exit application */
	XCloseDisplay(disp);
}

//-----------------------------------------------------------------------------
// CALLBACKS for LoServer
//-----------------------------------------------------------------------------

// see LoServer.h

/*
	 Switch active element in Flow Router if the Overlay will draw or not
	 user_data  = XnVFlowRouter
 */
int change_overlay_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
	//Overlayname: (string)(&argv[0]->s) 
	/* Testcall of Fuctions */
	if( argv[1]->i == 0 ){
		printf(" wait for overlay\n");
		//To change the active Element in the Flow has an disavantage: There's no timeout
		//till the next push will be detectable.
		//((XnVFlowRouter*)user_data)->SetActive( &pd1 );
		//	numpad_active = FALSE;
		overlay_active = FALSE;
		mouse_move_active = TRUE;
		g_pPointDetector->setMode(1);
		circle_active = FALSE;

		//g_pMainFlowRouter->SetActive(&pd1);
		//g_pMainFlowRouter->Activate(FALSE);
		g_pMainFlowRouter->SetActive(g_pVoidListener);
	}else{
		printf(" wait for interaction in overlay\n");

		//save name of active overlay, name holds till an other overlay will shown.
		strcpy(g_active_overlay, (char*)(&argv[0]->s) );

		// Push == Left_Mouse_Click
		//((XnVFlowRouter*)user_data)->SetActive( &pushLeftMouseButton );
		overlay_active = TRUE;

		//Set number of grid cells
		//((MyBox2*)user_data)->m_pTrackPad->SetItemCount( (XnInt32) (argv[2]->i), (XnInt32) (argv[3]->i) );
		g_pMySlider->SetItemCount( (XnInt32) (argv[3]->i), (XnInt32) (argv[2]->i) );

		/*
			 if( strcmp( g_active_overlay, "Civ4Citymenu.ui") == 0 ||
			 ( strcmp( g_active_overlay, "Browser.ui") == 0 )
			 ){  
			 circle_active = TRUE;
			 mouse_move_active = TRUE;
			 g_pPointDetector->setMode(1);
			 printf("Maus ist aktiv!!\n ");
		//g_pMainFlowRouter->SetActive(g_pCircle);
		g_pMainFlowRouter->SetActive(g_pDummy);
		}else{
		circle_active = FALSE;
		mouse_move_active = FALSE;
		g_pPointDetector->setMode(0);
		g_pMainFlowRouter->SetActive(g_pMySlider);
		}
		 */

		unsigned int optionFlags = argv[4]->i;

					printf("Flags: %u\n",optionFlags);

		/* Mouse moveable? */
		if( (optionFlags & 1) != 0){
			mouse_move_active = TRUE;
			g_pPointDetector->setMode(1);
			printf("Enable Mouse\n");
		}else{
			mouse_move_active = FALSE;
			g_pPointDetector->setMode(0);
			printf("Disable Mouse\n");
		}

		/* Enable Trackpad selection */
		if( (optionFlags & 2) != 0){
			g_pMainFlowRouter->SetActive(g_pMySlider);
			printf("Enable Trackpad\n");
		}else{
			g_pMainFlowRouter->SetActive(g_pDummy);
			printf("Disable Trackpad\n");
		}

		/* Enable Circle detection  */
		if( (optionFlags & 4) != 0){
			circle_active = TRUE;
			printf("Enable Circle\n");
		}else{
			circle_active = FALSE;
			printf("Disable Circle\n");
		}

	}
	fflush(stdout);

	return 1;
}


int general_message_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data) {
	//strcpy(g_active_overlay, (char*)(&argv[0]->s) );
	char* message = (char*)(&argv[0]->s);
	int mvalue = argv[1]->i;
	if( strcmp( message, "MoveAndScale" ) == 0 ){
		/*The values overlay_active, etc are not changed here.
			There has to invoke an other ocs message. */
		if( mvalue == 1){
			printf("Activate MoveAndScale.\n");
			g_pPointDetector->setMode(3);
		}else{
			g_pPointDetector->setMode(1);
		}
	}

	fflush(stdout);
	return 1;
}


//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

// this sample can run either as a regular sample, or as a client for multi-process (remote mode)
int main(int argc, char** argv)
{
	xn::Context context;
	XnVSessionGenerator* g_pSessionManager;
	XnBool bRemoting = FALSE;
	//g_currentPrimaryPosition = XnPoint3D();
	//g_currentSecondaryPosition = XnPoint3D();
	//loserver = new LoServer();

	//for keysend
	gdk_init (&argc, &argv);


	if (argc > 1)
	{
		// remote mode
		context.Init();
		printf("Running in 'Remoting' mode (Section name: %s)\n", argv[1]);
		bRemoting = TRUE;

		// Create multi-process client
		g_pSessionManager = new XnVMultiProcessFlowClient(argv[1]);

		XnStatus rc = ((XnVMultiProcessFlowClient*)g_pSessionManager)->Initialize();
		if (rc != XN_STATUS_OK)
		{
			printf("Initialize failed: %s\n", xnGetStatusString(rc));
			delete g_pSessionManager;
			return 1;
		}
	}
	else
	{
		// Local mode
		// Create context
		XnStatus rc = context.InitFromXmlFile(SAMPLE_XML_FILE);
		if (rc != XN_STATUS_OK)
		{
			printf("Couldn't initialize: %s\n", xnGetStatusString(rc));
			return 1;
		}

		// Create the Session Manager
		g_pSessionManager = new XnVSessionManager();
		rc = ((XnVSessionManager*)g_pSessionManager)->Initialize(&context, "Click", "RaiseHand");
		if (rc != XN_STATUS_OK)
		{
			printf("Session Manager couldn't initialize: %s\n", xnGetStatusString(rc));
			delete g_pSessionManager;
			return 1;
		}

		// Initialization done. Start generating
		context.StartGeneratingAll();
	}

	//No layer was active, init with empty name
	strcpy(g_active_overlay, "" );

	// Create the flow manager for first node
	g_pDummy = new XnVPushDetector;
	g_pMainFlowRouter = new XnVFlowRouter;
	g_pSecondFlowRouter = new XnVFlowRouter;
	g_pMyBox2 = new MyBox2(&uinp_fd, SecondaryOnPushCB, SecondaryOnStableCB, &sf);
	g_pMySlider = new MySlider2D( 7,7, 100, 100 ); 
	g_pPointDetector = new MyPointDetector( &sf );
	g_pPointDetector->setMode(1); // primary hand control mouse

	// get resolution
	getResolution(&display_width,&display_height);

	g_pMySlider->m_nItemSelectHandle = g_pMySlider->RegisterItemSelect(NULL, &TrackPad_ItemSelect);
	g_pMySlider->m_nPrimaryDestroyHandle = g_pMySlider->RegisterPrimaryPointDestroy(NULL, &TrackPad_PrimaryDestroy);

	// Register session callbacks
	g_pSessionManager->RegisterSession(NULL, &SessionStart, &SessionEnd, &SessionProgress);

	// Start catching signals for quit indications
	CatchSignals(&g_bQuit);

	// init & register wave control
	//XnVWaveDetector wc;
	//	wc.RegisterWave(NULL, OnWaveCB);
	//wc.RegisterPointUpdate(NULL, OnPointUpdate);
	//g_pSessionManager->AddListener(&wc);
	g_pSessionManager->AddListener(g_pPointDetector);

	//Void listener for inactive flows. (How deactivate flows?)
	//g_pVoidListener = new XnVMessageListener;
	g_pVoidListener = new XnVWaveDetector;

	// Register callbacks for both push detectors
	pd1.RegisterPush(NULL, PrimaryOnPushCB);
	pd1.RegisterStabilized(NULL, PrimaryOnStableCB);

	//Use stable detection as timeout...
	//pd1.SetPushImmediateOffset( DOUBLE_PUSH_TIME );//time to skip till stable will detect?
	//pd1.SetStableMaximumVelocity(1.0);//high value for certain detection
	//pd1.SetStableDuration(1);//1ms
	//pd1.SetStableDuration( DOUBLE_PUSH_TIME );

	// Connect tree:
	// A SecondaryFilter is connected to the SessionManager,
	// one PushDetector is connected to the SessionManager (will work on the primary point),
	// and the other PushDetector is connected to the SecondaryFilter (will work on the secondary point)
	//g_pMainFlowRouter->SetActive(&pd1);
	//g_pMainFlowRouter->Activate(FALSE);
	g_pMainFlowRouter->SetActive(g_pVoidListener);
	g_pSessionManager->AddListener(g_pMainFlowRouter);

	//g_pSecondFlowRouter->SetActive(&pd2);
	//sf.AddListener(g_pSecondFlowRouter);// own add by MyBox2
	g_pSessionManager->AddListener(&sf);

	// init and register circle control
	//g_pCircle = new XnVCircleDetector;
	g_pCircle = new MyCircleDetectorWithPush;
	g_pCircle->RegisterCircle(NULL, &CircleCB);
	g_pCircle->RegisterNoCircle(NULL, &NoCircleCB);
	//  g_pCircle->RegisterPrimaryPointCreate(NULL, &Circle_PrimaryCreate);
	//  g_pCircle->RegisterPrimaryPointDestroy(NULL, &Circle_PrimaryDestroy);
	g_pSessionManager->AddListener(g_pCircle);
	g_pSessionManager->AddListener(&pd1);

	//printf("Please perform focus gesture to start session\n");
	printf("Please perform focus gesture for first hand.\n After mouse cursor react, move second hand close to first hand and swipe both hands.\n");
	printf("Hit Ctrl+C to exit\n");
	setup_uinput_device();

	// Register LoServer callbacks
	lo_server_thread_add_method(loserver.st, "/overlay/manager/changeOverlay", "siiii", change_overlay_handler, g_pMyBox2);
	lo_server_thread_add_method(loserver.st, "/overlay/manager/generalMessage", "si", general_message_handler, NULL);

	// Main loop
	while (/*(!_kbhit()) &&*/ (!g_bQuit))
	{
		if (bRemoting)
		{
			((XnVMultiProcessFlowClient*)g_pSessionManager)->ReadState();
		}
		else
		{
			context.WaitAndUpdateAll();
			((XnVSessionManager*)g_pSessionManager)->Update(&context);
		}
	}

	//g_pSessionManager->RemoveListener(&pd1);
	g_pSessionManager->RemoveListener(g_pMainFlowRouter);
	//sf.RemoveListener(&pd2);
	sf.RemoveListener(g_pSecondFlowRouter);
	g_pSessionManager->RemoveListener(&sf);

	if(NULL != g_pSessionManager) delete g_pSessionManager;

	delete g_pPointDetector;
	delete g_pSecondFlowRouter;
	delete g_pMainFlowRouter;
	delete g_pMyBox2;
	delete g_pMySlider;
	delete g_pCircle;

	context.Shutdown();
	ioctl(uinp_fd, UI_DEV_DESTROY); 
	close(uinp_fd); 

	return 0;
}
