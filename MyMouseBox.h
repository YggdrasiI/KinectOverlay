/****************************************************************************
 *                                                                           *
 *   Nite 1.3 - Mouse Emulation Box                                          *
 *		Reduce MyBox.h Example to swipe detection without dawing.               *
 *                                                                           *
 *   Author:     Olaf Schulz                                                 *
 *                                                                           *
 ****************************************************************************/
/****************************************************************************
 *                                                                           *
 *   Nite 1.3 - Boxes                                                        *
 *                                                                           *
 *   Author:     Oz Magal                                                    *
 *                                                                           *
 ****************************************************************************/

/****************************************************************************
 *                                                                           *
 *   Nite 1.3	                                                            *
 *   Copyright (C) 2006 PrimeSense Ltd. All Rights Reserved.                 *
 *                                                                           *
 *   This file has been provided pursuant to a License Agreement containing  *
 *   restrictions on its use. This data contains valuable trade secrets      *
 *   and proprietary information of PrimeSense Ltd. and is protected by law. *
 *                                                                           *
 ****************************************************************************/

#ifndef MY_MOUSE_BOX_H
#define MY_MOUSE_BOX_H

// for sending ocs events
#include "OverlaySend.h"
#include "XnVSecondaryFilter.h"
#include "MySlider2D.h"

// A class for the box.
// It is a broadcaster, with its internal push detector and flow manager connected to it.
// The push detector allows detecting pushes regardless of is state (as long as it's active),
// and the flow manager is connected to either the swipe detector of the steady detector.
// This results in a box that responds to pushes (and raises the Leave event), and to swipes and steadys,
// in turn, which effect some internal state, which is reflected in the drawing.

class MyMouseBox : public XnVPointControl
{
	public:

		typedef void (XN_CALLBACK_TYPE *LeaveCB)(void* pUserCxt);
		typedef void (XN_CALLBACK_TYPE *PushCB)(XnFloat fVelocity, XnFloat fAngle, void* UserCxt);
		//typedef void (XN_CALLBACK_TYPE *SteadyCB)(XnUInt32 nId, XnFloat fStdDev, void* pUserCxt);

		// Create the MyMouseBox
		MyMouseBox(int * puinp_fd, PushCB activePushCB, XnVSecondaryFilter* psf) : XnVPointControl("MyMouseBox"), m_puinp_fd(puinp_fd)
	{
		// Create internal objects
		m_pInnerFlowRouter = new XnVFlowRouter;
		m_pPushDetector = new XnVPushDetector;
		m_pSwipeDetector = new XnVSwipeDetector;
		m_pTrackPad = new MySlider2D( 7,7, 100, 100 ); 
		/* default nbr of cells */7, 7,
			/*2x size in mm, 1x cell border size*/ /*300, 200, 0);*/

			m_pSwipeDetector->SetMotionSpeedThreshold(0.10);//0.10m/s
		m_pSwipeDetector->SetMotionTime(100);//100ms

		//m_pSteadyDetector = new XnVSteadyDetector;
		m_pSteadyDetector = new XnVSteadyDetector(100,10); //ignore arg1 ms, detect arg2 ms

		// Add the push detector and flow manager to the broadcaster
		//m_Broadcaster.AddListener(m_pInnerFlowRouter);
		//m_Broadcaster.AddListener(m_pPushDetector);
		psf->AddListener(m_pInnerFlowRouter);
		psf->AddListener(m_pPushDetector);

		// flow begins with the steady detector
		m_pInnerFlowRouter->SetActive(m_pSteadyDetector);

		// Register to notifications as broadcaster
		RegisterActivate(this, &Broadcaster_OnActivate);
		RegisterDeactivate(this, &Broadcaster_OnDeactivate);
		RegisterPrimaryPointCreate(this, &Broadcaster_OnPrimaryCreate);
		RegisterPrimaryPointDestroy(this, &Broadcaster_OnPrimaryDestroy);

		// Listen to inner activateable's events:
		// Push
		m_pPushDetector->RegisterPush(this, &Push_MyMousePush);
		//m_pPushDetector->RegisterPush(this, activePushCB);

		// Swipe
		m_pSwipeDetector->RegisterSwipe(this, &Swipe_ShowNumpad);
		/*
			 m_pSwipeDetector->RegisterSwipeUp(this, &Swipe_SwipeUp);
			 m_pSwipeDetector->RegisterSwipeDown(this, &Swipe_SwipeDown);
			 m_pSwipeDetector->RegisterSwipeLeft(this, &Swipe_SwipeLeft);
			 m_pSwipeDetector->RegisterSwipeRight(this, &Swipe_SwipeRight);
		 */
		// Steady
		//ERROR auf Laptop?!? No, Nite version problem .13 ok, 14 not
		m_pSteadyDetector->RegisterSteady(this, &Steady_OnSteady);

		//Add handler in MySlider2D object
		m_pTrackPad->m_nItemSelectHandle = m_pTrackPad->RegisterItemSelect(this, &TrackPad_ItemSelect);



		g_bmouse_down = FALSE;
		//g_bactive = false;
	}

		~MyMouseBox()
		{
			//m_Broadcaster.RemoveListener(m_pInnerFlowRouter);
			//m_Broadcaster.RemoveListener(m_pPushDetector);

			delete m_pInnerFlowRouter;
			delete m_pPushDetector;
			delete m_pSwipeDetector;
			delete m_pSteadyDetector;
			delete m_pTrackPad;
		}

		// Draw the box, with its frame
		void Draw()
		{
			// no drawing
		}


		// Change flow state between steady and swipe
		void SetTrackPadActive() {
			overlay_active = TRUE;
			numpad_active = TRUE;
			m_pInnerFlowRouter->SetActive(m_pTrackPad);
			m_pTrackPad->Activate(true);
		}

		void SetSteadyActive() {
			//if( numpad_active != TRUE )
			m_pInnerFlowRouter->SetActive(m_pSteadyDetector);
		}

		void SetSwipeActive() {
			civ4_change_overlay("",0);
			overlay_active = FALSE;
			numpad_active = FALSE;
			//if( numpad_active != TRUE )
			m_pInnerFlowRouter->SetActive(m_pSwipeDetector);
		}

		// Register/Unregister for MyMouseBox's event - Leave
		XnCallbackHandle RegisterLeave(void* UserContext, LeaveCB pCB)
		{
			XnCallbackHandle handle;
			m_LeaveCBs.Register(pCB, UserContext, &handle);
			return handle;
		}
		void UnregisterLeave(XnCallbackHandle handle)
		{
			m_LeaveCBs.Unregister(handle);
		}

		void Update(XnVMessage* pMessage)
		{
			XnVPointControl::Update(pMessage);
			m_Broadcaster.Update(pMessage);
		}

	private:
		// Callbacks for internal activateable's events:

		// The broadcaster (the MyMouseBox itself)
		static void XN_CALLBACK_TYPE Broadcaster_OnActivate(void* cxt)
		{
			MyMouseBox* box = (MyMouseBox*)(cxt);

			box->SetSteadyActive();
		}

		static void XN_CALLBACK_TYPE Broadcaster_OnDeactivate(void* cxt)
		{
			//MyMouseBox* box = (MyMouseBox*)(cxt);
		}

		static void XN_CALLBACK_TYPE Broadcaster_OnPrimaryCreate(const XnVHandPointContext* hand, const XnPoint3D& ptFocus, void* cxt)
		{
			//MyMouseBox* box = (MyMouseBox*)(cxt);
		}

		static void XN_CALLBACK_TYPE Broadcaster_OnPrimaryDestroy(XnUInt32 nID, void* cxt)
		{
			//MyMouseBox* box = (MyMouseBox*)(cxt);
		}

		// Push detector
		static void XN_CALLBACK_TYPE Push_MyMousePush(XnFloat fVelocity, XnFloat fAngle, void* cxt)
		{
			printf("Push in MyMouseBox!\n");
			MyMouseBox* box = (MyMouseBox*)(cxt);

			//g_bactive= g_bactive ^ true;

			if( numpad_active == TRUE ){
				//Dieser Zweig wird nie betreten werden, da statt dem Push die Zellselektierung aufgerufen wird?!
				//Set input center
				//((MySlider2D*)UserCxt)->Reposition(ptFocus);
				//((MySlider2D*)cxt)->Activate(true);
				//SetTrackPadActive();

				printf("Send Push Lo Event (in MyMousebox)\n");
				/* swaped push action of hands. Old was:
				// close overlay
				char uiname[] = "Civ4Numpad.ui";
				civ4_change_overlay(uiname,0);

				numpad_active == FALSE;
				overlay_active == FALSE;
				 */

				//box->SetSteadyActive();
				box->SetSwipeActive();
			}else if( overlay_active == TRUE ){
				box->send_mouse_down(BTN_LEFT);
				box->send_mouse_up(BTN_LEFT);
			}else {
				box->send_mouse_down(BTN_LEFT);
				box->send_mouse_up(BTN_LEFT);
			}

			// Leave the box
			//box->Leave();
		}

		// Swipe detector
		static void XN_CALLBACK_TYPE Swipe_ShowNumpad(XnVDirection eDir, XnFloat fVelocity, XnFloat fAngle, void* cxt)
		{
			printf("Show Numpad\n");
			MyMouseBox* box = (MyMouseBox*)(cxt);

			if( !overlay_active && !numpad_active ){ // if no other layout shown, (part after && currently redundant)
				char uiname[] = "Civ4Numpad.ui";
				civ4_change_overlay(uiname,1);
				box->SetTrackPadActive();
			}
		}

		static void XN_CALLBACK_TYPE Swipe_SwipeUp(XnFloat fVelocity, XnFloat fAngle, void* cxt)
		{
			printf("Up!\n");
			MyMouseBox* box = (MyMouseBox*)(cxt);

			box->SetSteadyActive();
		}

		static void XN_CALLBACK_TYPE Swipe_SwipeDown(XnFloat fVelocity, XnFloat fAngle, void* cxt)
		{
			printf("Down!\n");
			MyMouseBox* box = (MyMouseBox*)(cxt);

			box->SetSteadyActive();
		}

		static void XN_CALLBACK_TYPE Swipe_SwipeLeft(XnFloat fVelocity, XnFloat fAngle, void* cxt)
		{
			printf("Left!!\n");
			MyMouseBox* box = (MyMouseBox*)(cxt);

			//box->send_mouse_down(BTN_LEFT);
			//box->send_mouse_up(BTN_LEFT);

			box->SetSteadyActive();
		}

		static void XN_CALLBACK_TYPE Swipe_SwipeRight(XnFloat fVelocity, XnFloat fAngle, void* cxt)
		{
			printf("Right!\n");
			MyMouseBox* box = (MyMouseBox*)(cxt);

			box->send_mouse_down(BTN_RIGHT);
			box->send_mouse_up(BTN_RIGHT);
			//send_keysym(XK_g,0);

			box->SetSteadyActive();
		}

		// Steady detector
		static void XN_CALLBACK_TYPE Steady_OnSteady(XnFloat fVelocity, void* cxt)
		{
			printf("Steady\n");
			MyMouseBox* box = (MyMouseBox*)(cxt);

			box->SetSwipeActive();
		}


		// Inform all - leaving the box
		void Leave()
		{
			m_LeaveCBs.Raise();
		}

		// Callback for ItemSeletion in Trackpad
		static void XN_CALLBACK_TYPE TrackPad_ItemSelect(XnInt32 nXItem, XnInt32 nYItem, XnVDirection eDir, void* cxt)
		{
			printf("Select: %d,%d (%s)\n", nXItem, nYItem, XnVDirectionAsString(eDir));

			//Set input center //no Point available...
			//((MySlider2D*)cxt)->Reposition(ptFocus);
			//g_pMyMouseBox->SetSwipeActive();
			((MyMouseBox*)cxt)->SetSwipeActive();
			//Deactivate
			//((MySlider2D*)cxt)->Activate(false);
		}

		//+++++
		/*	static int uinp_fd = -1; 
				struct uinput_user_dev uinp; // uInput device structure 
				struct input_event event; // Input device structure 
		 */

		void send_mouse_down(int button)
		{
			if( button == BTN_LEFT )
				g_bmouse_down = TRUE;

			// Report BUTTON CLICK - PRESS event
			memset(&m_event, 0, sizeof(m_event));
			gettimeofday(&m_event.time, NULL);
			m_event.type = EV_KEY;
			m_event.code = button;//BTN_LEFT;
			m_event.value = 1;
			write(*m_puinp_fd, &m_event, sizeof(m_event));
			m_event.type = EV_SYN;
			m_event.code = SYN_REPORT;
			m_event.value = 0;
			write(*m_puinp_fd, &m_event, sizeof(m_event));
		}

		void send_mouse_up(int button)
		{
			// Report BUTTON CLICK - RELEASE event
			memset(&m_event, 0, sizeof(m_event));
			gettimeofday(&m_event.time, NULL);
			m_event.type = EV_KEY;
			m_event.code = button;//BTN_LEFT;
			m_event.value = 0;
			write(*m_puinp_fd, &m_event, sizeof(m_event));
			m_event.type = EV_SYN;
			m_event.code = SYN_REPORT;
			m_event.value = 0;
			write(*m_puinp_fd, &m_event, sizeof(m_event));

			if( button == BTN_LEFT )
				g_bmouse_down = FALSE;
		}

		//private:
		XnVPushDetector* m_pPushDetector;
		XnVFlowRouter* m_pInnerFlowRouter;
		XnVSwipeDetector* m_pSwipeDetector;
		XnVSteadyDetector* m_pSteadyDetector;
		XnVBroadcaster m_Broadcaster;
		XnVEvent m_LeaveCBs;

		bool g_bmouse_down;
		//bool g_bactive;
		int* m_puinp_fd;
		struct input_event m_event; // Input device structure 
	public:
		MySlider2D* m_pTrackPad;
};

#endif
