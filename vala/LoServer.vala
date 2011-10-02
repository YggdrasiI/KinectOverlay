using Gtk;
using Gdk;
using Lo;

namespace Overlay {

public class LoServer : GLib.Object {

/*
 Save the current time of an operation an block actions
 in an small period.
*/
//private static double timestamp = 0;
private const int64 blocking_time = 10000; //ms
private static DateTime begin;

public static bool is_blocked(){
					//long t = (long) time_t ();
					var now = new DateTime.now_local ();
					var diff = now.difference( begin );

					if ( diff < LoServer.blocking_time ){
						debug(@"Block LoServer operation, $(now.to_unix() + now.get_microsecond()/1E-6), $(diff) ");
					return true;
					}
					LoServer.begin = now;
					return false;
}


//+++ Some Handler

private Lo.ErrorHandler error = (num, msg, path) => {
	stdout.printf("liblo server error %d in path %s: %s\n", num, path, msg);
};


/*
First arg ( argv[0] ) :
	Name of overlay file (*.ui).
	To change the active overlay, use name="".
	Otherwise, no reaction happens, if the active overlay not match with name.
Second arg ( argv[1]->i ) :
	Action: 0 = hide overlay, 1 = show overlay, 2 = toggle visiblity of overlay.
*/
	public static Lo.MethodHandler changeOverlay = (path, types, argv, msg, userData) => {

					//if( LoServer.is_blocked() ) return 0;

					string uiname = (string)(&argv[0]->s);
					int action = argv[1]->i;
					debug(@" changeoverlay called. $uiname , $action\n");

					unowned OverlayManager* omanager = (OverlayManager*) userData;
					LoServer.uiname = uiname;

				 //omanager->showActiveOverlay();
				 //stdout.printf("Pointer in changeOverlay-Handler: %p\n", userData);
				 //stdout.printf("Pointer in changeOverlay-Handler: %p\n", omanager);


// use acces to omanger over  Overlay.omanager and uiname over LoSever.uiname (ugly :(  )
					if( action == 0){
						if( uiname != "") 
							callback_from_lo_server_thread = () => { Overlay.omanager.hideOverlay(LoServer.uiname); };
						else 
							callback_from_lo_server_thread = () => { Overlay.omanager.hideActiveOverlay(); };
					}
					if( action == 1 ){
						if( uiname != "") 
							callback_from_lo_server_thread = () => { Overlay.omanager.showOverlay(LoServer.uiname); };
						else 
							callback_from_lo_server_thread = () => { Overlay.omanager.showActiveOverlay(); };
					}
					if( action == 2){
						if( uiname != "")
							callback_from_lo_server_thread = () => { Overlay.omanager.toggleOverlay(LoServer.uiname); };
						else 
							callback_from_lo_server_thread = () => { Overlay.omanager.toggleActiveOverlay(); };
					}

				 Gtk.main_quit();

			return 0;
	};


	public static Lo.MethodHandler changeCell = (path, types, argv, msg, userData) => {

					if( LoServer.is_blocked() ) return 0;

					int row = argv[0]->i;
					int column = argv[1]->i;
					debug(@"change cell called. $row , $column\n");
					LoServer.row = row;
					LoServer.column = column;

				//	unowned OverlayManager* omanager = (OverlayManager*) userData;
				//	/*unowned*/ OverlayWindow active = omanager->getActiveOverlay();
					callback_from_lo_server_thread = () => {
							OverlayWindow active = Overlay.omanager.getActiveOverlay();
							if( active != null ) active.set_cell(LoServer.row, LoServer.column);
					};
				 Gtk.main_quit();

			return 0;
	};

	public static Lo.MethodHandler pushNamedCell = (path, types, argv, msg, userData) => {

					if( LoServer.is_blocked() ) return 0;

					string cellname = (string)(&argv[0]->s);
					debug(@"push cell called. $cellname\n");
					LoServer.cellname = cellname;

					callback_from_lo_server_thread = () => {
						OverlayWindow active = Overlay.omanager.getActiveOverlay();
						if( active != null ){
							Widget w =	active.set_named_cell(LoServer.cellname);
							if( w == null ) return; 
							EventButton eb = EventButton();
							eb.button = 1;
							//w.button_release_event(eb);
							((OverlayEventBox)w).my_button_release_event(EventButton());
						}
					};
				 Gtk.main_quit();

			return 0;
	};

	public static Lo.MethodHandler pushCell = (path, types, argv, msg, userData) => {

					if( LoServer.is_blocked() ) return 0;

					int row = argv[0]->i;
					int column = argv[1]->i;
					debug(@"push cell called. $row , $column\n");
					LoServer.row = row;
					LoServer.column = column;

					//unowned OverlayManager* omanager = (OverlayManager*) userData;
					///*unowned*/ OverlayWindow active = omanager->getActiveOverlay();


					callback_from_lo_server_thread = () => {
						OverlayWindow active = Overlay.omanager.getActiveOverlay();
						if( active != null ){
							Widget w =	active.set_cell(LoServer.row, LoServer.column);
							if( w == null ) return; 
							EventButton eb = EventButton();
							eb.button = 1;
							//w.button_release_event(eb);
							((OverlayEventBox)w).my_button_release_event(EventButton());
						}
					};
				 Gtk.main_quit();

			return 0;
	};

//+++++++++++++++++++++++++++

		public ServerThread st;
		private OverlayManager* omanager;
		public static string uiname;
		public static string cellname;
		public static int row;
		public static int column;
		public LoServer(string port, OverlayManager* omanager ){
			this.omanager = omanager;

			st = new Lo.ServerThread (port, error);
			//st.add_method(null, null, genericHandler, null);
			//st.add_method("/quit", "", quitHandler, null);
			 //add_method (string? path, string? typespec, MethodHandler h, void* user_data);
			//				Pfad, Signatur, Handler, Objekt
			st.add_method("/overlay/manager/changeOverlay", "si", changeOverlay, this.omanager);
			st.add_method("/overlay/window/selectCell", "ii", changeCell, this.omanager);
			st.add_method("/overlay/window/pushCell", "ii", pushCell, this.omanager);
			st.add_method("/overlay/window/pushNamedCell", "s", pushNamedCell, this.omanager);

			st.start();

			LoServer.begin = new DateTime.now_local ();
		}

		/* Delete all added methodes befor call this. (?!)
		*/
		public void stopServerThread(){
			 st.del_method("/overlay/manager/changeOverlay", "si");
			 st.del_method("/overlay/window/selectCell", "ii");
			 st.del_method("/overlay/window/pushCell", "ii");
			 st.del_method("/overlay/window/pushNamedCell", "s");
			 st.stop();
		}


	}

}
