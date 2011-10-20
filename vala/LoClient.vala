/*using Gtk;
using Gdk;*/
using Lo;

namespace Overlay {

/*
 Broadcast Messages with the state of the Overlay
*/
public class LoClient : GLib.Object {

private OverlayManager omanager;

public LoClient(string? host, string port,  OverlayManager omanager){
	this.omanager = omanager;
	this.lo_address = new Lo.Address(host, port);
}

//+++ List of possible Messages 

public	void broadcastChangeOverlay(){
 OverlayWindow winMain;
	if( (winMain = omanager.getActiveOverlay()) == null){
		debug("active window is null.");
		return;
	}
		var message = new Lo.Message();

		message.add_string( winMain.get_name());
		message.add_int32( winMain.is_show?1:0 );
		message.add_int32((int) winMain.n_rows);
		message.add_int32((int) winMain.n_columns);

/*
1 - Mouse movement active
2 - Trackpad selection active
4 - Circle detection active
*/
		int optionFlags = 0;
		optionFlags += (winMain.react_on_mouse?1:0);
		optionFlags += (winMain.trackpad_selection?2:0);
		optionFlags += (winMain.circle_detection?4:0);

		message.add_int32((int) optionFlags);

		message.send(this.lo_address, "/overlay/manager/changeOverlay");
	}


public void broadcastCellClicked(){
 OverlayWindow winMain;
	if( (winMain = omanager.getActiveOverlay()) == null){
		debug("active window is null.");
		return;
	}
		var message = new Lo.Message();

		message.add_string( winMain.get_name());
		message.add_int32( winMain.row_active);
		message.add_int32( winMain.column_active);
		debug(@"Send pushCell $(winMain.row_active) , $(winMain.column_active) ");

		message.send(this.lo_address, "/overlay/window/pushCell");
	}


public	void broadcastStringMessage(string messagepath, string messagename, int messagevalue){
		var lomessage = new Lo.Message();

		lomessage.add_string( messagename );
		lomessage.add_int32( messagevalue );

		lomessage.send(this.lo_address, messagepath );
	}


//+++++++++++++++++++++++++++

		private Lo.Address lo_address;

	}

}
