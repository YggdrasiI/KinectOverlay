/****************************************************************************
*                                                                           *
*   Implementation of XnVPointControl.                                      *
*   Provides function for smoothing point values and some sample            *
*   callbacks.                                                              *
*   Author:     Olaf Schulz                                                 *
*                                                                           *
****************************************************************************/


#ifndef MY_POINT_DETECTOR_H
#define MY_POINT_DETECTOR_H

#include <math.h>

#include "MouseActions.h"

//above keysend routines do not work. Use my keysend methods.
		#include "X11/keysym.h"
    #include "keysend.h"

#include <XnUtils.h>
#include <XnV3DVector.h>
#include <XnVPointControl.h>
#include "XnVSecondaryFilter.h"

static const int KC_LEFT = 113; 
static const int KC_RIGHT = 114; 
static const int KC_UP = 111; 
static const int KC_DOWN = 116; 
static const int KC_PAGE_UP = 112; 
static const int KC_PAGE_DOWN = 117; 

/*
 The class MyPointDetector saves the coordinates of two hands. Furthermore the coordinates
 of one hand will smothing and used as mouse input, if activted.
*/

//Mouse Move Stuff:
/*
	 Smoothing ansatz:
	 X_i 	- input values
	 X 		- smoothed value
	 N = MAXCOORDS, C*ω_1 = weight, C*ω_N = weight2, ω=factor
	 X = Σ_{i=1}^N ω_i * X_i, ω_i := C*ω_{i-1},  Σ_i ω_i = 1
	 Add new input with 
	 X -= ω_2* Xlist[i];
	 Xlist[i] = Xneu;
	 X += Xlist[i];
	 X *= ω;
	 i = (i+1)%N;
 */

// Matlab program to detect weight factor for other N:
/*
	 function val = findFactor(N, w) % w- weight of first component
	 coeffs = w*ones(1,N+1); 
	 coeffs(end) = -1; 
	 p= @(x) polyval(coeffs,x);

	 r = roots(coeffs);
	 val = r( find(r == real(r)) );
	 val = val( find( val > 0 )); 

	 % Test
	 p(val)
	 weights = []; 
	 for i=1:N
	 weights(i) = w*val^i;
	 end

	 xx = sum(weights);

	 fprintf('Reelle Nullstelle: %3.20f\n %i-te Potenz: %3.20f\nSumme: %f\n', val, N, val^N,xx);
	 fprintf('Startgewicht: %3.20f\nEndgewicht: %3.20f\nFaktor: %3.20f\n', weights(1), weights(end), val);

	 end
 */

// Scale&Move Stuff
// For Moving. Minimal displacement for reaction
static const double MOVE_PERCENT_LIMIT_X = 0.5;
static const double MOVE_PERCENT_LIMIT_Y = 0.25;

// For one hand Scaling. Minimal displacement for reaction
static const double MOVE_PERCENT_LIMIT_Z = 1.0;
// for two hand zoom ( measure difference of both hands)
static const double ZOOM_IN_MINIMAL_RATIO = 1.1;//>1
static const double ZOOM_IN_MAXIMAL_RATIO = 1.6;//> ZOOM_IN_MINIMAL_RATIO
static const double ZOOM_OUT_MAXIMAL_RATIO = 0.9;//<1
static const double ZOOM_OUT_MINIMAL_RATIO = 0.4;//< ZOOM_OUT_MAXIMAL_RATIO

static const int ACTION_TIMEOUT = 100; // amount of time during two events
static const bool ALLOW_PRESSING_MULTIPLE_KEYS = true; //up to three pressed keys.
static const bool HOLD_PRESSED_KEYS_DOWN = true;
static const int PRESSED_KEYCODE_SIZE = 3; //maximal number of simultaneous pressable keys.
static const int KEYPRESS_TIMEOUT = 80; //amount of time a key is pressed in milliseconds, if HOLD_PRESSED_KEYS_DOWN = false;
static const int LC_FACTOR_DENOMINATOR = 32; // a = (1/LC_FACTOR_DENOMINATOR)*a_new + (1-1/LC_FACTOR_DENOMINATOR)*a;


class SmoothingPoint3D{
public: 
				XnPoint3D* m_ppoint;
// Mouse Move Stuff
static const int MAXCOORDS=20;
//Weightfactors for MAXCOORDS=20 and C=0.25
static const double WEIGHT1 = 0.20048524438888434829;
static const double WEIGHT2 = 0.00302543704876776214;//= C*weight1^MAXCOORDS
static const double FACTOR = 0.80194097755553739315;

static const double W1 = 19.0/20.0;
static const double W2 = 1.0/20.0;

private:
	//			double m_positionArr[MAXCOORDS][3];
				XnPoint3D m_points[MAXCOORDS];
				int m_posI;
				double m_scaleX, m_scaleY, m_scaleZ;


public:
	//Contructors and Destructor			
	SmoothingPoint3D(XnPoint3D* point){
		SmoothingPoint3D(point,1.0,1.0,1.0);
	}

	SmoothingPoint3D(XnPoint3D* point, float scaleX, float scaleY, float scaleZ){
			m_ppoint = point;
			m_posI=0;
			m_scaleX = scaleX;
			m_scaleY = scaleY;
			m_scaleZ = scaleZ;

			for(int i=0;i<MAXCOORDS;i++){
				m_points[i] = xnCreatePoint3D(point->X, point->Y, point->Z);
				/*m_positionArr[i][0] = point->X;
				m_positionArr[i][1] = point->Y;
				m_positionArr[i][2] = point->Z;*/
			}
}
	virtual ~SmoothingPoint3D(){
		//delete m_ppoint;//wrong, m_ppoint not init in this class
	}
public:
		void updateXXX( XnPoint3D input ){
				m_ppoint->X *= W1;
				m_ppoint->X += W2*input.X*m_scaleX;
				m_ppoint->Y *= W1;
				m_ppoint->Y += W2*input.Y*m_scaleY;
				m_ppoint->Z *= W1;
				m_ppoint->Z += W2*input.Z*m_scaleZ;

		}

		void update( XnPoint3D input ){
				m_ppoint->X = m_ppoint->X - WEIGHT2*m_points[m_posI].X;
				m_points[m_posI].X = input.X*m_scaleX;
				m_ppoint->X *= FACTOR;
				m_ppoint->X += WEIGHT1*m_points[m_posI].X;

				m_ppoint->Y = m_ppoint->Y - WEIGHT2*m_points[m_posI].Y;
				m_points[m_posI].Y = input.Y*m_scaleY;
				m_ppoint->Y *= FACTOR;
				m_ppoint->Y += WEIGHT1*m_points[m_posI].Y;
				
				m_ppoint->Z = m_ppoint->Z - WEIGHT2*m_points[m_posI].Z;
				m_points[m_posI].Z = input.Z*m_scaleZ;
				m_ppoint->Z *= FACTOR;
				m_ppoint->Z += WEIGHT1*m_points[m_posI].Z;

				m_posI = (m_posI+1) % MAXCOORDS;

		}

		XnPoint3D getSmooth(){
			return *m_ppoint;
		}

		XnPoint3D getUnsmooth(){
			return m_points[m_posI];
		}

};

/*
 Same as SmoothingPoint3D for many Points.
 This case use a linear combination to avoid rounding errors and an big array for the input buffer.
*/
class SmoothingPoint3D_2{
private:
	//			double m_positionArr[MAXCOORDS][3];
				double m_scaleX, m_scaleY, m_scaleZ;
				int m_prelude;

public: 
				XnPoint3D* m_ppoint;
				bool m_bactive;
// Mouse Move Stuff
//Weightfactors for MAXCOORDS=1000 and C=1/100
//static const double WEIGHT1 = 0.00990099477839656399 - 0.00000047734400513625;
//static const double FACTOR = 0.99009947783965646106;
static const double WEIGHT1 = 9999.0/10000.0;
static const double WEIGHT2 = 1.0/10000.0;

	//Contructors and Destructor			
	SmoothingPoint3D_2(XnPoint3D* point){
		SmoothingPoint3D_2(point,1.0,1.0,1.0);
	}

	SmoothingPoint3D_2(XnPoint3D* point, float scaleX, float scaleY, float scaleZ){
			m_ppoint = point;
			m_scaleX = WEIGHT2*scaleX;
			m_scaleY = WEIGHT2*scaleY;
			m_scaleZ = WEIGHT2*scaleZ;
			m_prelude = 0;
			m_bactive = true;
	}
	virtual ~SmoothingPoint3D_2(){
		//delete m_ppoint;//wrong, m_ppoint not init in this class
	}
public:
		void update( XnPoint3D input ){
		if( ! m_bactive ) return;
		/* //big rounding errors...
				m_ppoint->X *= WEIGHT1;
				m_ppoint->X += WEIGHT2*input.X*m_scaleX;
				m_ppoint->Y *= WEIGHT1;
				m_ppoint->Y += WEIGHT2*input.Y*m_scaleY;
				m_ppoint->Z *= WEIGHT1;
				m_ppoint->Z += WEIGHT2*input.Z*m_scaleZ;
		*/

		/* Use other weights for the first steps */
		if( m_prelude < 10 ){
						m_prelude++;
						m_ppoint->X += (input.X - m_ppoint->X)*(0.2*m_scaleX/WEIGHT2);
						m_ppoint->Y += (input.Y - m_ppoint->Y)*(0.2*m_scaleY/WEIGHT2);
						m_ppoint->Z += (input.Z - m_ppoint->Z)*(0.2*m_scaleZ/WEIGHT2);

		}else{
						m_ppoint->X += (input.X - m_ppoint->X)*m_scaleX;
						m_ppoint->Y += (input.Y - m_ppoint->Y)*m_scaleY;
						m_ppoint->Z += (input.Z - m_ppoint->Z)*m_scaleZ;
		}

				//printf("Longtime: %f, %f, %f\n", m_ppoint->X, m_ppoint->Y, m_ppoint->Z);
		}

		XnPoint3D getSmooth(){
			return *m_ppoint;
		}

		void Reposition(const XnPoint3D& ptCenter){
			m_ppoint->X = ptCenter.X;
			m_ppoint->Y = ptCenter.Y;
			m_ppoint->Z = ptCenter.Z;
		}

};


// callback for a new position of any hand
void XN_CALLBACK_TYPE MyPointDetector_OnPointUpdate(const XnVHandPointContext* pContext, void* cxt);



// +++++++++++++++++ Class definition ++++++++++++++++++++

class XNV_NITE_API MyPointDetector :
	public virtual XnVPointControl
{
private:
				XnCallbackHandle m_nRegisterPointUpdateHandle;
				XnVSecondaryFilter* m_psf;
				//bool m_mouse_active = false;
				XnPoint3D* m_pmouse;

				// Mouse Move Stuff
				int m_lastAbsoluteX;
				int m_lastAbsoluteY;
				int m_currentAbsoluteX;
				int m_currentAbsoluteY;
			  int m_nmode; //  Point Update call [primary|seondary|both]Update CB

public:
		XnPoint3D m_primaryPosition;
	  XnPoint3D m_secondaryPosition;
		SmoothingPoint3D* m_psmoothPrimaryPosition;
		SmoothingPoint3D* m_psmoothSecondaryPosition;
		SmoothingPoint3D* m_pMousePosition; //maps to m_psmoothPrimaryPosition or m_psmoothSecondaryPosition

		XnPoint3D m_scale_initPrimaryPosition;
	  XnPoint3D m_scale_initSecondaryPosition;
		XnPoint3D m_scale_midpoint;
		XnPoint3D m_scale_initMidpoint;
		double m_scale_radius;
		double m_scale_initRadius;
		double m_move_limitX;
		double m_move_limitY;
		double m_move_limitZ;
		XnUInt64 m_lastActionTimestamp;
		int m_pressed_keycode[PRESSED_KEYCODE_SIZE];
		int m_pressed_keycode_index;
		bool m_bAllowOnlyEightDirections;

		XnPoint3D m_primaryPositionLongtime;
		SmoothingPoint3D_2* m_psmoothPrimaryPositionLongtime;

		MyPointDetector(XnVSecondaryFilter* psf ) : XnVPointControl( ){
			m_nmode = 0;
			m_psf = psf;
			m_primaryPosition = XnPoint3D();
			m_secondaryPosition = XnPoint3D();

			// 4,2 works good
			m_psmoothPrimaryPosition = new SmoothingPoint3D(&m_primaryPosition,4.0,2.0,1.0);
			m_psmoothSecondaryPosition = new SmoothingPoint3D(&m_secondaryPosition,4.0,2.0,1.0);

//			m_psmoothPrimaryPosition = new SmoothingPoint3D(&m_primaryPosition,5.0,2.5,1.0);
//			m_psmoothSecondaryPosition = new SmoothingPoint3D(&m_secondaryPosition,5.0,2.5,1.0);


			m_primaryPositionLongtime = XnPoint3D();
			m_psmoothPrimaryPositionLongtime = new SmoothingPoint3D_2(&m_primaryPositionLongtime,4.0,2.0,1.0);

			m_scale_initPrimaryPosition = XnPoint3D();
			m_scale_initSecondaryPosition = XnPoint3D();
			m_scale_midpoint = XnPoint3D();
			m_scale_initMidpoint = XnPoint3D();
			m_pmouse = &m_primaryPosition;
			m_lastActionTimestamp = 0;

			// Mouse move stuff
			m_lastAbsoluteX = 0;
		  m_lastAbsoluteY = 0;
			m_currentAbsoluteX = 0;
			m_currentAbsoluteY = 0;
			m_bAllowOnlyEightDirections = false;
			for(int i=0; i<PRESSED_KEYCODE_SIZE; i++){
				m_pressed_keycode[i] = -1;
			}
			m_pressed_keycode_index = 0;
		
			m_nRegisterPointUpdateHandle = RegisterPointUpdate(this, MyPointDetector_OnPointUpdate);
		}

	virtual ~MyPointDetector(){

    // Unregister internal callbacks (no used)
    if(NULL != m_nRegisterPointUpdateHandle)
      UnregisterPointUpdate(m_nRegisterPointUpdateHandle);

		delete m_psmoothPrimaryPosition;
		delete m_psmoothSecondaryPosition;

	}

// ++++++++++ Used Callbacks in MyPointDetector ++++++
void PointUpdate(const XnVHandPointContext* pContext){
				XnUInt32 nContext = pContext->nID;
				if( nContext == m_psf->nPrimary ){
								m_psmoothPrimaryPosition->update( pContext->ptPosition );
								m_psmoothPrimaryPositionLongtime->update( pContext->ptPosition );
								if( m_nmode == 1 ){
										MouseCB();
										return;
								}
				}else{
								m_psmoothSecondaryPosition->update( pContext->ptPosition );
								if( m_nmode == 2 ){
										MouseCB();
										return;
								}
				}

				if( m_nmode == 3 ){
								ScaleAndMoveCB( *(m_psmoothPrimaryPosition->m_ppoint), *(m_psmoothSecondaryPosition->m_ppoint));
								return;
				}
}

//ToDo: Implementation as general Callback routines
void MouseCB(){

			m_currentAbsoluteX = (int)( m_pMousePosition->m_ppoint->X);
			m_currentAbsoluteY = (int)( m_pMousePosition->m_ppoint->Y);

			float move_factor;
			double zdiff = m_psmoothPrimaryPositionLongtime->m_ppoint->Z - m_pMousePosition->m_ppoint->Z;
			//printf("Diff: %f \t longZ: %f \t shortZ: %f\n",zdiff, m_psmoothPrimaryPositionLongtime->m_ppoint->Z, m_pMousePosition->m_ppoint->Z);

			/* Reduce movement if hand is close to the sensor. TODO: Define constant for if-header */
			if( zdiff > 150 ){
				move_factor = 0.25;
			}else{
				move_factor = 1.0;
			}


			//Move cursor
			//check, if movement should reduced to eight directions, if right mousebutton pressed
			if( m_bAllowOnlyEightDirections ){
				double arc = atan2( m_currentAbsoluteY-m_lastAbsoluteY, m_currentAbsoluteX-m_lastAbsoluteX );
				double d = move_factor* fmax( fabs(m_currentAbsoluteX-m_lastAbsoluteX), fabs(m_currentAbsoluteY-m_lastAbsoluteY) );
				int intervall = floor( arc/M_PI*4 + 0.5);
				switch( intervall ){
					case 4: //same like -4
					case -4: move_cursor( -d, 0 ); break;
					case -3: move_cursor( -d, d ); break;
					case -2: move_cursor( 0, d ); break;
					case -1: move_cursor( d, d ); break;
					case 0: move_cursor( d, 0 ); break;
					case 1: move_cursor( d, -d ); break;
					case 2: move_cursor( 0, -d ); break;
					case 3: move_cursor( -d, -d ); break;
					default: printf("Error in switch block\n");	
				}

			}else{

			//printf("Move Cursor! %i, %i\n",m_currentAbsoluteX-m_lastAbsoluteX, m_lastAbsoluteY-m_currentAbsoluteY);
			move_cursor(
				move_factor *( m_currentAbsoluteX-m_lastAbsoluteX ),
				move_factor *( m_lastAbsoluteY-m_currentAbsoluteY )
			);
			}

			m_lastAbsoluteX = m_currentAbsoluteX;
			m_lastAbsoluteY = m_currentAbsoluteY; 
}


void ScaleAndMoveCB(XnPoint3D primaryPoint, XnPoint3D secondaryPoint){

	XnUInt64 nNow;
  xnOSGetHighResTimeStamp(&nNow); 

	if( nNow- m_lastActionTimestamp < ACTION_TIMEOUT ) return;
//	m_lastActionTimestamp = nNow;//no, only update if action detected

//		bool nozoom = false;					 

		double dPrimX = (primaryPoint.X - m_scale_initPrimaryPosition.X);
		double dPrimY = (primaryPoint.Y - m_scale_initPrimaryPosition.Y);


		if( dPrimX < -m_move_limitX ){
			printf("Move Left\n");
			releaseKey( KC_RIGHT);
			move_left(dPrimX,dPrimY);
			m_lastActionTimestamp = nNow;
			//nozoom = true;
		} else if( dPrimX > m_move_limitX ){
			printf("Move Right\n");
			releaseKey( KC_LEFT);
			move_right(dPrimX,dPrimY);
			m_lastActionTimestamp = nNow;
			//nozoom = true;
		} else {
			releaseKey( KC_LEFT);
			releaseKey( KC_RIGHT);
		}

		if( dPrimY < -m_move_limitY ){
			printf("Move Down\n");
			releaseKey( KC_UP);
			move_down(dPrimX,dPrimY);
			m_lastActionTimestamp = nNow;
			//nozoom = true;
		} else if( dPrimY > m_move_limitY ){
			printf("Move Up\n");
			releaseKey( KC_DOWN);
			move_up(dPrimX,dPrimY);
			m_lastActionTimestamp = nNow;
			//nozoom = true;
		} else {
			releaseKey( KC_UP);
			releaseKey( KC_DOWN);
		}


		//if( nozoom ) return;
		/*
		double radiusSecondary = (m_scale_initMidpoint.X - secondaryPoint.X)
												*(m_scale_initMidpoint.X - secondaryPoint.X)
									     + (m_scale_initMidpoint.Y - secondaryPoint.Y)
										    *(m_scale_initMidpoint.Y - secondaryPoint.Y);
		radiusSecondary = sqrt( radiusSecondary );	
		double ratio = radiusSecondary/m_scale_initRadius;
		//printf("Ratio: %f\n", ratio);

	  if( ratio > ZOOM_IN_MINIMAL_RATIO && ratio < ZOOM_IN_MAXIMAL_RATIO){
			printf("Zoom In\n");
			releaseKey( KC_PAGE_DOWN );
			zoomIn( ratio );
			m_lastActionTimestamp = nNow;
		}else	if(	ratio < ZOOM_OUT_MAXIMAL_RATIO && ratio > ZOOM_OUT_MINIMAL_RATIO){
			printf("Zoom Out\n");
			releaseKey( KC_PAGE_UP );
			zoomOut( ratio );
			m_lastActionTimestamp = nNow;
		}else{
			releaseKey( KC_PAGE_UP );
			releaseKey( KC_PAGE_DOWN );
		}
		*/

		/* One hand zoom */
		double dPrimZ = (primaryPoint.Z - m_scale_initPrimaryPosition.Z);
		if( dPrimZ < -m_move_limitZ ){
			printf("Zoom In\n");
			releaseKey( KC_PAGE_DOWN );
			zoomIn( ZOOM_IN_MINIMAL_RATIO );
			m_lastActionTimestamp = nNow;
		} else if( dPrimZ > m_move_limitZ ){
			printf("Zoom Out\n");
			releaseKey( KC_PAGE_UP );
			zoomOut( ZOOM_OUT_MAXIMAL_RATIO );
			m_lastActionTimestamp = nNow;
		} else {
			releaseKey( KC_PAGE_DOWN );
			releaseKey( KC_PAGE_UP );
		}

}


void copyPoint(XnPoint3D* p1, XnPoint3D* p2){
	p2->X = p1->X;
	p2->Y = p1->Y;
	p2->Z = p1->Z;
}


void setMode(int mode){
	m_nmode = mode;
	if( mode == 1){
		m_pMousePosition = m_psmoothPrimaryPosition;
	}else
	if( mode == 2){
		m_pMousePosition = m_psmoothSecondaryPosition;
	}

	if( mode == 3 ){
		initScale();
		scale_active = TRUE;
	}else{
		//deactivated pressed keys
		releaseKey( -1 );

		scale_active = FALSE;
	}
}


int getMode(){
	return m_nmode;
}



private:
void initScale(){
		copyPoint( m_psmoothPrimaryPosition->m_ppoint, &m_scale_initPrimaryPosition);
		copyPoint( m_psmoothSecondaryPosition->m_ppoint, &m_scale_initSecondaryPosition);
		m_scale_initMidpoint.X = (m_scale_initPrimaryPosition.X + m_scale_initSecondaryPosition.X)/2;
		m_scale_initMidpoint.Y = (m_scale_initPrimaryPosition.Y + m_scale_initSecondaryPosition.Y)/2;
		m_scale_initMidpoint.Z = (m_scale_initPrimaryPosition.Z + m_scale_initSecondaryPosition.Z)/2;
		m_scale_initRadius = (m_scale_initPrimaryPosition.X - m_scale_initSecondaryPosition.X)
												*(m_scale_initPrimaryPosition.X - m_scale_initSecondaryPosition.X)
									     + (m_scale_initPrimaryPosition.Y - m_scale_initSecondaryPosition.Y)
										    *(m_scale_initPrimaryPosition.Y - m_scale_initSecondaryPosition.Y);
		m_scale_initRadius = sqrt( m_scale_initRadius )/2;

		m_move_limitX = m_scale_initRadius*MOVE_PERCENT_LIMIT_X;
		m_move_limitY = m_scale_initRadius*MOVE_PERCENT_LIMIT_Y;
		m_move_limitZ = m_scale_initRadius*MOVE_PERCENT_LIMIT_Z;

printf("Diagonale: %f", 2*m_scale_initRadius);
		if( m_scale_initRadius < 0.5) {
		//Points to near
			setMode(0);
		}
}

void updateScale(){
 		//Linear combination of old and current midpoint
		/*
		m_scale_initMidpoint.X = (m_psmoothPrimaryPosition.X + m_psmoothSecondaryPosition.X)/16
														+ m_scale_initMidpoint.X*b;
		m_scale_initMidpoint.Y = (m_psmoothPrimaryPosition.Y + m_psmoothSecondaryPosition.Y)/16
														+ m_scale_initMidpoint.Y*b;
		m_scale_initMidpoint.Z = (m_psmoothPrimaryPosition.Z + m_psmoothSecondaryPosition.Z)/16
														+ m_scale_initMidpoint.Z*b;
	  */

		float a = 1.0/LC_FACTOR_DENOMINATOR;
		float b = 1-a;

		m_scale_initPrimaryPosition.X = m_scale_initPrimaryPosition.X*b 
																	+ m_psmoothPrimaryPosition->m_ppoint->X*a;
		m_scale_initPrimaryPosition.Y = m_scale_initPrimaryPosition.Y*b 
																	+ m_psmoothPrimaryPosition->m_ppoint->Y*a;
		m_scale_initPrimaryPosition.Z = m_scale_initPrimaryPosition.Z*b 
																	+ m_psmoothPrimaryPosition->m_ppoint->Z*a;

		m_scale_initSecondaryPosition.X = m_scale_initSecondaryPosition.X*b 
																	+ m_psmoothSecondaryPosition->m_ppoint->X*a;
		m_scale_initSecondaryPosition.Y = m_scale_initSecondaryPosition.Y*b 
																	+ m_psmoothSecondaryPosition->m_ppoint->Y*a;
		m_scale_initSecondaryPosition.Z = m_scale_initSecondaryPosition.Z*b 
																	+ m_psmoothSecondaryPosition->m_ppoint->Z*a;

		m_scale_initMidpoint.X = (m_scale_initPrimaryPosition.X + m_scale_initSecondaryPosition.X)/2;
		m_scale_initMidpoint.Y = (m_scale_initPrimaryPosition.Y + m_scale_initSecondaryPosition.Y)/2;
		m_scale_initMidpoint.Z = (m_scale_initPrimaryPosition.Z + m_scale_initSecondaryPosition.Z)/2;
	
		m_scale_initRadius = (m_scale_initPrimaryPosition.X - m_scale_initSecondaryPosition.X)
												*(m_scale_initPrimaryPosition.X - m_scale_initSecondaryPosition.X)
									     + (m_scale_initPrimaryPosition.Y - m_scale_initSecondaryPosition.Y)
										    *(m_scale_initPrimaryPosition.Y - m_scale_initSecondaryPosition.Y);
		m_scale_initRadius = sqrt( m_scale_initRadius )/2;

}

void updateMidpoint(){
 //Linear combination of old and current point
 		float a = 1.0/LC_FACTOR_DENOMINATOR;
		float b = 1-a;

		m_scale_initPrimaryPosition.X = m_scale_initPrimaryPosition.X*b 
																	+ m_psmoothPrimaryPosition->m_ppoint->X*a;
		m_scale_initPrimaryPosition.Y = m_scale_initPrimaryPosition.Y*b 
																	+ m_psmoothPrimaryPosition->m_ppoint->Y*a;
		m_scale_initPrimaryPosition.Z = m_scale_initPrimaryPosition.Z*b 
																	+ m_psmoothPrimaryPosition->m_ppoint->Z*a;
}


void pressKey(int keycode){
	if(! HOLD_PRESSED_KEYS_DOWN){
		//simple case: press and release key
		keycodesend2(keycode,KEYPRESS_TIMEOUT);
		return;
	}

		//check, if already pressed
		for(int i=0; i<=m_pressed_keycode_index; i++){
			if( m_pressed_keycode[i] == keycode ) return;
		}
		if( !ALLOW_PRESSING_MULTIPLE_KEYS ) releaseKey(keycode);
		if( m_pressed_keycode_index >= PRESSED_KEYCODE_SIZE ) return;

		keycodesend(keycode, 1);
		m_pressed_keycode[m_pressed_keycode_index] = keycode;
		m_pressed_keycode_index++;
		//m_pressed_keycode_index = (m_pressed_keycode+1) % PRESSED_KEYCODE_SIZE;
}

void releaseKey(int keycode){
	if(! HOLD_PRESSED_KEYS_DOWN){
		//simple case: Keys released immediately and can't be hold.
		return;
	}

	if( keycode == -1 ){
		//release all keys
		for(int i=0; i<=m_pressed_keycode_index; i++){
			keycodesend(m_pressed_keycode[i], 0);
			m_pressed_keycode[i] = -1; //redundant
		}
		m_pressed_keycode_index = 0;
	}else{
		//release only keycode
		for(int i=0; i<=m_pressed_keycode_index; i++){
			if( m_pressed_keycode[i] == keycode ){
				keycodesend(m_pressed_keycode[i], 0);
				for(int j=i+1; j<=m_pressed_keycode_index; j++){
					m_pressed_keycode[j-1] = m_pressed_keycode[j];
				}
				m_pressed_keycode[m_pressed_keycode_index] = -1;
				m_pressed_keycode_index = i;
				return;
			}
		}
	}
}

void zoomIn(double factor){
//				send_mouse_down(BTN_FORWARD);
//			send_mouse_up(BTN_FORWARD);
// keysend3(XK_Next, 0 , 0, 100);
//	keycodesend2(112,KEYPRESS_TIMEOUT);
	pressKey(KC_PAGE_UP);
	updateScale();
}

void zoomOut(double factor){
//				send_mouse_down(BTN_BACK);
//				send_mouse_up(BTN_BACK);
// keysend3(XK_Prior, 0 , 0, 100);
	//keycodesend2(117,KEYPRESS_TIMEOUT);
	pressKey(KC_PAGE_DOWN);
	updateScale();
}


void move_left(double dX, double dY){
 //send_keycode(KEY_LEFT);
// keysend3(XK_Left, 0 , 0, 50);
	//keycodesend2(113,KEYPRESS_TIMEOUT);
	pressKey(KC_LEFT);
	updateMidpoint();
}
void move_right(double dX, double dY){
 //send_keycode(KEY_RIGHT);
 //keysend3(XK_Right, 0 , 0, 50);
 //	keycodesend2(114,KEYPRESS_TIMEOUT);
	pressKey(KC_RIGHT);
	updateMidpoint();
}
void move_up(double dX, double dY){
 //send_keycode(KEY_UP);
 //keysend3(XK_Up, 0 , 0, 50);
 //keycodesend2(111,KEYPRESS_TIMEOUT);
	pressKey(KC_UP);
	updateMidpoint();
}
void move_down(double dX, double dY){
 //send_keycode(KEY_DOWN);
 //keysend3(XK_Down, 0 , 0, 50);
 //keycodesend2(116,KEYPRESS_TIMEOUT);
	pressKey(KC_DOWN);
	updateMidpoint();
}

};//End Class


void XN_CALLBACK_TYPE MyPointDetector_OnPointUpdate(const XnVHandPointContext* pContext, void* cxt)
{
				((MyPointDetector*)(cxt))->PointUpdate(pContext);
}

#endif // MY_POINT_DETECTOR_H
