/****************************************************************************
*                                                                           *
*   Nite 1.3 - Mouse Emulation Box                                          *
*		Reduce MyBox.h Example to swipe detection without dawing.               *
*                                                                           *
*   Author:     Olaf Schulz, 2011                                           *
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

// A class for the box.
// It is a broadcaster, with its internal push detector and flow manager connected to it.
// The push detector allows detecting pushes regardless of is state (as long as it's active),
// and the flow manager is connected to either the swipe detector of the steady detector.
// This results in a box that responds to pushes (and raises the Leave event), and to swipes and steadys,
// in turn, which effect some internal state, which is reflected in the drawing.

class MyBox2 : public XnVPointControl
{
public:

	typedef void (XN_CALLBACK_TYPE *LeaveCB)(void* pUserCxt);
	typedef void (XN_CALLBACK_TYPE *PushCB)(XnFloat fVelocity, XnFloat fAngle, void* UserCxt);
	typedef void (XN_CALLBACK_TYPE *StabilizedCB)(XnFloat fVelocity, void* UserCxt);
	//typedef void (XN_CALLBACK_TYPE *SteadyCB)(XnUInt32 nId, XnFloat fStdDev, void* pUserCxt);

	// Create the MyBox2
	MyBox2(int * puinp_fd, PushCB secondaryPushCB, StabilizedCB secondaryStableCB, XnVSecondaryFilter* psf) : XnVPointControl("MyBox2"), m_puinp_fd(puinp_fd)
	{
		m_psf = psf;
		// Create internal objects
		m_pInnerFlowRouter = new XnVFlowRouter;
		m_pPushDetector = new XnVPushDetector;
		m_pSwipeDetector = new XnVSwipeDetector;

		//m_pSwipeDetector->SetMotionSpeedThreshold(0.10);//0.10m/s
		//m_pSwipeDetector->SetMotionTime(100);//100ms
	
		//m_pSteadyDetector = new XnVSteadyDetector;
		m_pSteadyDetector = new XnVSteadyDetector(100,10); //ignore arg1 ms, detect arg2 ms

		//m_pInnerFlowRouter.SetActive(m_pPushDetector);
		m_psf->AddListener(m_pPushDetector);

		// flow begins with the steady detector
		m_pInnerFlowRouter->SetActive(m_pSteadyDetector);

		// Register to notifications as broadcaster
		RegisterActivate(this, &Broadcaster_OnActivate);
		RegisterDeactivate(this, &Broadcaster_OnDeactivate);
		RegisterPrimaryPointCreate(this, &Broadcaster_OnPrimaryCreate);
		RegisterPrimaryPointDestroy(this, &Broadcaster_OnPrimaryDestroy);

		// Connect PushDetector with Push
		m_pPushDetector->RegisterPush(this, secondaryPushCB);
		m_pPushDetector->RegisterStabilized(this, secondaryStableCB);

		// Swipe
		m_pSwipeDetector->RegisterSwipe(this, &Swipe_ShowNumpad);
		/*
		m_pSwipeDetector->RegisterSwipeUp(this, &Swipe_SwipeUp);
		m_pSwipeDetector->RegisterSwipeDown(this, &Swipe_SwipeDown);
		m_pSwipeDetector->RegisterSwipeLeft(this, &Swipe_SwipeLeft);
		m_pSwipeDetector->RegisterSwipeRight(this, &Swipe_SwipeRight);
		*/
		// Steady
		m_pSteadyDetector->RegisterSteady(this, &Steady_OnSteady);

		//psf->AddListener(m_pSwipeDetector);//wrong
		m_psf->AddListener(m_pInnerFlowRouter);
	}

	~MyBox2()
	{
		//m_psf->RemoveListener(m_pInnerFlowRouter);
		m_psf->RemoveListener(m_pPushDetector);
		m_psf->RemoveListener(m_pInnerFlowRouter);

		delete m_pInnerFlowRouter;
		delete m_pPushDetector;
		delete m_pSwipeDetector;
		delete m_pSteadyDetector;
	}


	// Change flow state between steady and swipe
	/*
	void SetNumpadActive() {
			overlay_active = TRUE;
			numpad_active = TRUE;
			m_pInnerFlowRouter->SetActive(m_pTrackPad);
			m_pTrackPad->Activate(true);
	}
	*/

	void SetSteadyActive() {
		m_pInnerFlowRouter->SetActive(m_pSteadyDetector);
	}

	void SetSwipeActive() {
		m_pInnerFlowRouter->SetActive(m_pSwipeDetector);
	}

	// Register/Unregister for MyBox2's event - Leave
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

	// The broadcaster (the MyBox2 itself)
	static void XN_CALLBACK_TYPE Broadcaster_OnActivate(void* cxt)
	{
		MyBox2* box = (MyBox2*)(cxt);

		box->SetSteadyActive();
	}

	static void XN_CALLBACK_TYPE Broadcaster_OnDeactivate(void* cxt)
	{
		//MyBox2* box = (MyBox2*)(cxt);
	}

	static void XN_CALLBACK_TYPE Broadcaster_OnPrimaryCreate(const XnVHandPointContext* hand, const XnPoint3D& ptFocus, void* cxt)
	{
		//MyBox2* box = (MyBox2*)(cxt);
	}

	static void XN_CALLBACK_TYPE Broadcaster_OnPrimaryDestroy(XnUInt32 nID, void* cxt)
	{
		//MyBox2* box = (MyBox2*)(cxt);
	}

	// Push detector
	/*
	static void XN_CALLBACK_TYPE Push_MyMousePush(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		printf("Push in MyBox2!\n");
		MyBox2* box = (MyBox2*)(cxt);

		if( numpad_active == TRUE ){

			printf("Send Push Lo Event (in MyMousebox)\n");

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
	}*/

	// Swipe detector
	static void XN_CALLBACK_TYPE Swipe_ShowNumpad(XnVDirection eDir, XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		MyBox2* box = (MyBox2*)(cxt);

	//if( eDir == DIRECTION_FORWARD || eDir == DIRECTION_BACKWARD) return;
	if( eDir != DIRECTION_RIGHT ) return;

		if( scale_active ) return ;

		if( !overlay_active ){ 
			printf("Show Numpad\n");
			char uiname[] = "Civ4Numpad.ui";
			civ4_change_overlay(uiname,1);
			box->SetSteadyActive();
		}else{
			printf("Hide Overlay\n");
			char uiname[] = "";
			civ4_change_overlay(uiname,0);
			box->SetSteadyActive();
		}
	}

	static void XN_CALLBACK_TYPE Swipe_SwipeUp(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		printf("Up!\n");
		MyBox2* box = (MyBox2*)(cxt);

		box->SetSteadyActive();
	}

	static void XN_CALLBACK_TYPE Swipe_SwipeDown(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		printf("Down!\n");
		MyBox2* box = (MyBox2*)(cxt);

		box->SetSteadyActive();
	}

	static void XN_CALLBACK_TYPE Swipe_SwipeLeft(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		printf("Left!!\n");
		MyBox2* box = (MyBox2*)(cxt);

		box->SetSteadyActive();
	}

	static void XN_CALLBACK_TYPE Swipe_SwipeRight(XnFloat fVelocity, XnFloat fAngle, void* cxt)
	{
		printf("Right!\n");
		MyBox2* box = (MyBox2*)(cxt);

		box->SetSteadyActive();
	}

	// Steady detector
	static void XN_CALLBACK_TYPE Steady_OnSteady(XnFloat fVelocity, void* cxt)
	{
		printf("Steady\n");
		MyBox2* box = (MyBox2*)(cxt);

		box->SetSwipeActive();
	}


	// Inform all - leaving the box
	void Leave()
	{
		m_LeaveCBs.Raise();
	}

// Callback for ItemSeletion in Trackpad
//static void XN_CALLBACK_TYPE TrackPad_ItemSelect(XnInt32 nXItem, XnInt32 nYItem, XnVDirection eDir, void* cxt)

//+++++

//private:
	XnVPushDetector* m_pPushDetector;
	XnVFlowRouter* m_pInnerFlowRouter;
	XnVSwipeDetector* m_pSwipeDetector;
	XnVSteadyDetector* m_pSteadyDetector;
	XnVBroadcaster m_Broadcaster;
	XnVEvent m_LeaveCBs;
	XnVSecondaryFilter* m_psf;

	int* m_puinp_fd;
	struct input_event m_event; // Input device structure 
};

#endif
