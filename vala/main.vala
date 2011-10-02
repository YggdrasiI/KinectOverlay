/*
 * Overlay for better control with Kinect
 */
using Gtk;
using Gdk;
using GLib;

namespace Overlay {

public delegate void MainCallback();

static OverlayManager omanager;
static KeybindingManager kmanager;
public MainCallback callback_from_lo_server_thread = null;
 
static int main (string[] args) {     
    Gtk.init (ref args);
 

		omanager = new OverlayManager();
    kmanager = new KeybindingManager();
		var loserver = new LoServer("7776", omanager);
		//kmanager.bind("<Ctrl><Alt>V", () => { omanager.show_keybind_handler("Civ4BTgrid.ui"); } );

		omanager.loadButtons("Civ4list.ui");
		omanager.loadOverlay("Civ4BTgrid.ui");
		omanager.loadOverlay("Civ4Unitgrid.ui");
		omanager.loadOverlay("Civ4Numpad.ui");

		//omanager.showOverlay("Civ4Numpad.ui");

		//kmanager.bind("<Ctrl><Alt>V", () => { omanager.show_keybind_handler(null); } );
		//kmanager.bind("<Ctrl><Alt>V", () => { omanager.show_keybind_handler("Civ4Numpad.ui"); } );
		kmanager.bind("<Ctrl><Alt>V", () => { omanager.show_keybind_handler("Civ4Unitgrid.ui"); } );
		kmanager.bind("<Ctrl><Alt>X", () => { omanager.show_keybind_handler("Civ4Numpad.ui"); } );
    //Gtk.main ();
		while(true){
			Gtk.main();
			if( callback_from_lo_server_thread != null){
				callback_from_lo_server_thread();
				callback_from_lo_server_thread = null;
			}else{
				break;
			}
		}

		loserver.stopServerThread();

    return 0;
}

} //end namespace Overlay

/* Extern C routines */
extern int keysend(uint keysym, int modifiers);
extern int keysend2(uint keysym, uint modsym1, uint modsym2);

