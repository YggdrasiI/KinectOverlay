/****************************************************************************
*                                                                           *
*   Implementation of XnVSelectableSlider2D.                                *
*                                                                           *
*                                                                           *
*   Author:     Olaf Schulz                                                 *
*                                                                           *
****************************************************************************/

/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef MY_SLIDER_H
#define MY_SLIDER_H

#include "XnVSelectableSlider2D.h"

// +++++++++++++++++ Class definition ++++++++++++++++++++

class XNV_NITE_API MySlider2D :
	public XnVSelectableSlider2D
{
private:
				XnCallbackHandle m_nItemHoverHandle;
				XnCallbackHandle m_nValueChangeHandle;
				XnCallbackHandle m_nPrimaryCreateHandle;

public:
				static const XnFloat ms_fDefaultSliderSize = 80; //100;	// = 450
				XnCallbackHandle m_nPrimaryDestroyHandle;
				XnCallbackHandle m_nItemSelectHandle;
				XnInt32 m_XIndex;
				XnInt32 m_YIndex;

		MySlider2D( XnInt32 nXItems, XnInt32 nYItems,
		XnFloat fSliderSizeX = ms_fDefaultSliderSize, XnFloat fSliderSizeY = ms_fDefaultSliderSize,
		XnFloat fBorderWidth = 0, const XnChar* strName = "XnVSelectableSlider2D") :
		XnVSelectableSlider2D( nXItems, nYItems, fSliderSizeX, fSliderSizeY, fBorderWidth, strName){

		m_XIndex = 0;
		m_YIndex = 0;

		SetValueChangeOnOffAxis(true);

	  // Register for the Hover event of the TrackPad
  	m_nItemHoverHandle = RegisterItemHover(this, &TrackPad_ItemHover);
  	// Register for the Value Change event of the TrackPad
  	m_nValueChangeHandle = RegisterValueChange(NULL, &TrackPad_ValueChange);
  	// Register for the Select event of the TrackPad
  	//m_nItemSelectHandle = RegisterItemSelect(this, &TrackPad_ItemSelect);

    // Register for Input Start event of the TrackPad
    m_nPrimaryCreateHandle = RegisterPrimaryPointCreate(this, &TrackPad_PrimaryCreate);
    // Register for Input Stop event of the TrackPad
    //m_nPrimaryDestroyHandle = RegisterPrimaryPointDestroy(NULL, &TrackPad_PrimaryDestroy);
		}

	virtual ~MySlider2D(){

    // Unregister for the Hover event of the TrackPad
    if(NULL != m_nItemHoverHandle)
      UnregisterItemHover(m_nItemHoverHandle);
    // Unregister for the Value Change event of the TrackPad
    if(NULL != m_nValueChangeHandle)
      UnregisterValueChange(m_nValueChangeHandle);
		// Unregister for the Select event of the TrackPad
		if (NULL != m_nItemSelectHandle)
			UnregisterItemSelect(m_nItemSelectHandle);
    // Unregister for Input Stop event of the TrackPad
    if(NULL != m_nPrimaryDestroyHandle)
      UnregisterPrimaryPointDestroy(m_nPrimaryDestroyHandle);
    // Unregister for Input Start event of the TrackPad
    if(NULL != m_nPrimaryCreateHandle)
      UnregisterPrimaryPointCreate(m_nPrimaryCreateHandle);
	}

/* extend existing method
 Resize Trackpad size, if many arguments given.
*/
	void SetItemCount(XnUInt32 nItemXCount, XnUInt32 nItemYCount){
			XnVSelectableSlider2D::SetItemCount(nItemXCount, nItemYCount);
			SetSliderSize(
					(nItemXCount<8)?ms_fDefaultSliderSize:1.5*ms_fDefaultSliderSize ,
					(nItemYCount<8)?ms_fDefaultSliderSize:1.5*ms_fDefaultSliderSize
										);
	}



private:

// ++++++++++ Used Callbacks in MySlider2D ++++++
// Fixed Callbacks for MySilder. Connect on creation
// Trackpad
static void XN_CALLBACK_TYPE TrackPad_ValueChange(XnFloat fXValue, XnFloat fYValue, void* cxt)
{
//    printf("Value changed: %f, %f\n", fXValue, fYValue);
/*
  if(TRUE == g_isPrintValueChange)
  g_fXValue = fXValue;
  g_fYValue = fYValue;
*/
}

static void XN_CALLBACK_TYPE TrackPad_ItemHover(XnInt32 nXItem, XnInt32 nYItem, void* cxt)
{
    printf("Hover: %d,%d\n", nXItem, nYItem);
		((MySlider2D*)cxt)->m_XIndex = nXItem;
		((MySlider2D*)cxt)->m_YIndex = nYItem;

	//Send Lo Message, map coordinate system
	select_cell((int) ( (((MySlider2D*)cxt)->GetItemYCount())-1-nYItem), (int) nXItem );
}

/* in main
static void XN_CALLBACK_TYPE TrackPad_ItemSelect(XnInt32 nXItem, XnInt32 nYItem, XnVDirection eDir, void* cxt)
{
	printf("Select: %d,%d (%s)\n", nXItem, nYItem, XnVDirectionAsString(eDir));

	MySlider2D* pMySlider = ((MySlider2D*)cxt);
	pMySlider->m_pFlowRouter->SetActive(  pMySlider->m_pPd  );

	//Set input center //no Point available...
	//g_pMyMouseBox->SetSwipeActive();
	//Deactivate
	//((MySlider2D*)cxt)->Activate(false);
}*/

static void XN_CALLBACK_TYPE TrackPad_PrimaryCreate(const XnVHandPointContext* cxt, const XnPoint3D& ptFocus, void* UserCxt)
{
  printf("TrackPad input has started!!!, point ID: [%d] ", cxt->nID);

	//Set input center
	//((MySlider2D*)UserCxt)->Reposition(ptFocus);//Dummerweise bei der falschen Hand?!

	/*
  printf("Starting point position: [%f],[%f],[%f]\n", cxt->ptPosition.X, cxt->ptPosition.Y, cxt->ptPosition.Z);
  g_isInputStarted = TRUE;
*/
}
/*
static void XN_CALLBACK_TYPE TrackPad_PrimaryDestroy(XnUInt32 nID, void* UserCxt)
{
  printf("TrackPad input has stopped!!!\n");

	(()UserCxt)
}
*/

};//End Class



#endif // MY_SLIDER_H
