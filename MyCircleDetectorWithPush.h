/****************************************************************************
*                                                                           *
*   Implementation of XnVCircleDetector.                                    *
*   The combination of circle and push detection was abandon. Replace       *
*   occurrence of this class with XnVCircleDetection.                       *
*                                                                           *
*   Author:     Olaf Schulz, 2011                                           *
*                                                                           *
****************************************************************************/

/*******************************************************************************
*                                                                              *
*   PrimeSense NITE 1.3                                                        *
*   Copyright (C) 2010 PrimeSense Ltd.                                         *
*                                                                              *
*******************************************************************************/


#ifndef MY_CIRCLE_DETECTOR_WITH_PUSH_H
#define MY_CIRCLE_DETECTOR_WITH_PUSH_H

#include <XnVCircleDetector.h>
//#include <XnVPushDetector.h>

// +++++++++++++++++ Class definition ++++++++++++++++++++

class XNV_NITE_API MyCircleDetectorWithPush :
	public virtual XnVCircleDetector
{
/*private:
				XnCallbackHandle m_nRegisterPushHandle;
				XnCallbackHandle m_nOnCircleHandle;
				XnCallbackHandle m_nOnNoCircleHandle;
*/
public:

		MyCircleDetectorWithPush( ) :
		XnVCircleDetector( ){

		}

	virtual ~MyCircleDetectorWithPush(){

    // Unregister internal callbacks (no used)
   // if(NULL != m_nItemHoverHandle)
     // UnregisterItemHover(m_nItemHoverHandle);
	}

private:

// ++++++++++ Used Callbacks in MyCircleDetectorWithPush ++++++


};//End Class



#endif // MY_CIRCLE_DETECTOR_WITH_PUSH_H
