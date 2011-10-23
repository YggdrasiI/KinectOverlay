/*
 */
using Gtk;
using Gdk;
using GLib;

namespace Overlay {

	public class OverlayManager {

		private OverlayWindow active = null;
		public Gee.HashMap<string, OverlayWindow> gridMap;
		public Gee.HashMap<string, OverlayEventBox> eventBoxMap;
		private Gtk.Builder builder;
		private string defaultOverlayName = null; //store first overlay, which is loaded (not showed).
		private TimeoutSource refreshOverlayStatusTimeout;
		public LoClient loclient;

		public OverlayManager(){
			this.loclient = new LoClient(null,"7778", this);
			this.gridMap = new Gee.HashMap<string, OverlayWindow>();
			this.eventBoxMap = new Gee.HashMap<string, OverlayEventBox>();

			this.builder = new Builder ();

			this.refreshOverlayStatusTimeout = new TimeoutSource(10000);
			this.refreshOverlayStatusTimeout.attach(null);
			this.refreshOverlayStatusTimeout.set_callback( this.refreshOverlayStatus );

		}

		public void show_keybind_handler(string? name)
		{
			if( name != null ){
				var tmp = this.getOverlay(name);
				if( this.active != tmp ){
					this.hideActiveOverlay();
					this.active = tmp;
					this.showActiveOverlay();
				}else
					//this.active.toggle_show();
					this.toggleActiveOverlay();
			}else{
				if( this.active == null ) return;
				//this.active.toggle_show();
				this.toggleActiveOverlay();
			}
		}


		public bool loadButtons(string name){

			try {
				builder.add_from_file ("overlays/"+name);
				var list = builder.get_object("button_list") as Gtk.Container; 
				list.foreach( (w) => {
						var eb = w as OverlayEventBox;
						if( eb != null ){
						//eb.init();
						this.eventBoxMap.set( eb.box_name, eb);
						}
						} );

			} catch (Error e) {
				stderr.printf ("Could not load UI: %s\n", e.message);
				return false;
			} 

			return true;
		}


		public bool loadOverlay(string name){

			if( this.gridMap.has_key(name) ){
				debug("Overlay Object already loaded.");
				return false;
			}

			try {
				var builder = new Builder ();
				builder.add_from_file ("overlays/"+name);
				builder.connect_signals (null);
				var window = builder.get_object ("overlay") as OverlayWindow;
				//var table = builder.get_object("buttontable") as Gtk.Table; 

				window.init();
				window.set_name(name);
				// gtkbuilder omits constructor ?!
				/* recursive init element with Initable interface */
				//window.init_children(window);


				this.gridMap.set(name,window);
				if( this.active == null) this.active = window;
				if( this.defaultOverlayName == null) this.defaultOverlayName = name;
			} catch (Error e) {
				stderr.printf ("Could not load UI: %s\n", e.message);
				return false;
			} 

			return true;
		}


		public bool showActiveOverlay(){
			if( this.active == null) return false;
			this.active.show();

			this.loclient.broadcastChangeOverlay( );
			return true;
		}

		public bool showOverlay(string name){
			if( this.active != null )
				this.active.hide();

			if( name == "defaultOverlay" ){
				// Show default overlay or overlay, which was given as startup paramter
				name = this.defaultOverlayName; 
			}

			if( this.gridMap.has_key(name) || this.loadOverlay(name) ){
				this.active = this.gridMap.get(name);
				this.active.show(); //or toggle_show() ?!
			}else{
				debug(@"Overlay $name not found.\n");
				return false;
			}

			this.loclient.broadcastChangeOverlay( );
			return true;
		}

		public OverlayWindow getActiveOverlay(){
			return this.active;
		}

		public bool setActiveOverlay( OverlayWindow active ){
			this.active = active;
			return this.active.is_show;
		}

		public OverlayWindow? getOverlay(string name){
			if( this.gridMap.has_key(name) || this.loadOverlay(name) )
				return this.gridMap.get(name);
			else
				return null;
		}

		/* hide active overlay */
		public void hideActiveOverlay(){
			if( this.active == null ) return;
			this.active.hide();
			this.loclient.broadcastChangeOverlay( );
		}

		/* Only hide, if active */
		public void hideOverlay(string name){
			if( this.active == this.gridMap.get(name) ) this.active.hide();
			this.loclient.broadcastChangeOverlay( );
		}

		public void toggleActiveOverlay(){
			this.active.toggle_show();
			this.loclient.broadcastChangeOverlay( );
		}

		public void toggleOverlay(string name){
			if( name == "defaultOverlay" ){
				// Show default overlay or overlay, which was given as startup paramter
				name = this.defaultOverlayName; 
			}

			if( this.active == this.gridMap.get(name) ) this.active.toggle_show();
			else{
				// overlay not visible
				this.showOverlay(name);
			}
		}

		public bool refreshOverlayStatus(){
			//this.loclient.broadcastChangeOverlay( );

			return true;
		}

		private unowned TableChild? getTableChild( Widget w){
			var table = w.get_parent() as Gtk.Table;
			if( table == null ) return null;
			foreach(unowned Gtk.TableChild tc  in table.children ){
				if( tc.widget == w ) return tc;
			}
			return null;
		}

		public void set_cell_indizes(OverlayEventBox eb){
			unowned TableChild? tc = getTableChild( eb );
			if( tc == null ) return;

			//! There's no guaranty, that active is the right window
			this.active.row_active = tc.top_attach;
			this.active.column_active = tc.left_attach;

		}

	}// End Class OverlayManager


}

