/* 
    Simple example of sending an OSC message using oscpack.
*/

#ifndef OVERLAY_SEND_H
#define OVERLAY_SEND_H

#include "osc/OscOutboundPacketStream.h"
#include "ip/UdpSocket.h"

//#include <string.h>

#define ADDRESS "127.0.0.1"
#define PORT 7776

#define OUTPUT_BUFFER_SIZE 1024

static bool overlay_active = FALSE;
//static bool numpad_active = FALSE;
static bool mouse_move_active = TRUE;
static bool circle_active = FALSE;
static bool scale_active = TRUE;

/* action:
		0 -> hide overlay, 1 -> show overlay, 2 -> toggle visiblity
*/
void civ4_change_overlay(char* overlayName, int action)
{
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, PORT ) );
    
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    p << osc::BeginBundleImmediate
        << osc::BeginMessage( "/overlay/manager/changeOverlay" ) 
						//<< "Civ4.ui" << (int) action << osc::EndMessage
						<< overlayName << (int) action << osc::EndMessage
        << osc::EndBundle;
    
    transmitSocket.Send( p.Data(), p.Size() );
}

void select_cell(int x, int y)
{
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, PORT ) );
    
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    p << osc::BeginBundleImmediate
        << osc::BeginMessage( "/overlay/window/selectCell" ) 
						<< (int) x << (int) y << osc::EndMessage
        << osc::EndBundle;
    
    transmitSocket.Send( p.Data(), p.Size() );
}


void push_cell(int x, int y)
{
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, PORT ) );
    
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    p << osc::BeginBundleImmediate
        << osc::BeginMessage( "/overlay/window/pushCell" ) 
						<< (int) x << (int) y << osc::EndMessage
        << osc::EndBundle;
    
    transmitSocket.Send( p.Data(), p.Size() );
}

void push_named_cell(char* cellName){
    UdpTransmitSocket transmitSocket( IpEndpointName( ADDRESS, PORT ) );
    
    char buffer[OUTPUT_BUFFER_SIZE];
    osc::OutboundPacketStream p( buffer, OUTPUT_BUFFER_SIZE );
    
    p << osc::BeginBundleImmediate
        << osc::BeginMessage( "/overlay/window/pushNamedCell" ) 
						<< cellName << osc::EndMessage
        << osc::EndBundle;
    
    transmitSocket.Send( p.Data(), p.Size() );
}
#endif // OVERLAY_SEND_H
