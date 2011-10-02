using Gtk;
using Gdk;
using GLib;

namespace Overlay {

public class OverlayWindow : Gtk.Window, Initable {

public bool is_show;
public bool react_on_mouse {get; set;}
public int prefered_position {get; set;}
private int width;
private int height;
public bool maximizeOverlay {get; set;}
public string win_name {get; set;}
public uint n_rows; //todo: set public readonly;
public uint n_columns;
public int row_active = -1;
public int column_active = -1;
//private double button_scale = 0.7;
public Gee.HashMap<string, OverlayEventBox> eventBoxMap;
//private Gtk.TableChild tableChild = null;


public OverlayWindow(){
//this.maximize();
	this.win_name = "noname";
	this.react_on_mouse = true;
init();
}

public void init(){
  this.eventBoxMap = new Gee.HashMap<string, OverlayEventBox>();
	this.n_rows = 0;
	this.n_columns = 0;
	this.win_name = this.name;
debug(@"Win name: $(this.win_name)");
	this.set_gravity( Gdk.Gravity.SOUTH_EAST );
	//this.set_gravity( Gdk.Gravity.NORTH_WEST );

	this.set_keep_above(true);
	if( this.maximizeOverlay )	this.maximize();
	this.set_accept_focus(false);

	Gdk.Color bg_color;
	Gdk.Color.parse("#000000", out bg_color);
	this.modify_bg(Gtk.StateType.NORMAL, bg_color);

	//this.foreach( table_cb ); 
	foreach(unowned Widget w  in this.get_children() ){
		var t = w as Gtk.Table;
		if( t != null ){
			this.replace_placeholders( t );
			this.scaleGridImages(t);
		}
	}

//	this.scaleGridImages(table);
  this.destroy.connect (Gtk.main_quit);
  //this.show_all ();
	this.hide();

}

/*
private void table_cb(Widget widget){
	Table table = widget as Table;
	if( table != null){
		this.replace_placeholders(ref table);
		this.scaleGridImages(ref table);
		}
}*/

/*
 Skaliert die Bilder innerhalb einer Tabellenzelle.
 Falls die Zelle mehr als 1x1-Gitterzellen groß ist, wird dies
 bei der Größenberechnung ignoriert.
 Erst aufrufen, nachdem die Platzhalter ausgetauscht wurden.
*/
public void scaleGridImages( Gtk.Table table){

	this.get_size(out this.width, out this.height);
	uint rows = table.n_rows;
	uint columns = table.n_columns;
	this.n_rows += table.n_rows;
	this.n_columns += table.n_columns;
	//table.foreach(cb_eventbox);

	foreach(unowned Gtk.TableChild tc  in table.children ){

		// Allow all resizing operations of the table cell.
		//Ahgl, variables not exists....
		//tc.xoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
		//tc.yoptions = GTK_EXPAND | GTK_SHRINK | GTK_FILL;
		//stdout.printf(@"Tablechild properties: $(tc.left_attach), $(tc.right_attach), $(tc.top_attach), $(tc.bottom_attach), $(tc.xpadding), $(tc.ypadding)\n");

		var eb = tc.widget as OverlayEventBox;
		if( eb != null ){
				int spanRows = tc.bottom_attach - tc.top_attach ;
				int spanColumns = tc.right_attach - tc.left_attach ;
				spanRows = (spanRows>0)?spanRows:1;
				spanColumns = (spanColumns>0)?spanColumns:1;

				var image = eb.get_child() as  Gtk.Image; 
				if( image == null ) continue;
				/*
				int dim = (int) (GLib.Math.fmin( spanColumns*width/rows, spanRows*height/columns) * button_scale);
				image.set_from_pixbuf( 
					image.get_pixbuf().scale_simple( dim, dim, Gdk.InterpType.BILINEAR)
				);
				*/
				double W =  spanColumns*width/columns;
				double H = spanRows*height/rows;
				double w = (double) image.get_pixbuf().get_width();
				double h = (double) image.get_pixbuf().get_height();
				if( H/h > W/w){
					h = h*W/w;
					w = W;
				}else{
					w = w*H/h;
					h = H;
				}

				image.set_from_pixbuf( 
					image.get_pixbuf().scale_simple( (int)w, (int)h, Gdk.InterpType.BILINEAR)
				);
		}
	}

}

/*
 Replace the PlaceholderBoxes.
// Furthermore, the function connect the widges of an table cell with
// the TableCild object.
*/
private void replace_placeholders(Gtk.Table table){

	Gee.List<Widget> to_remove = new Gee.LinkedList<Widget>();

	foreach(unowned Gtk.TableChild tc  in table.children ){
				if( (tc.widget is PlaceholderBox) ){
					var ph = tc.widget as PlaceholderBox;
					var eb = omanager.eventBoxMap.get(ph.box_name);
					eb = eb.copyBox();

					if( eb == null ){
									stderr.printf(@"Button with Label $(ph.box_name) not found.\n");
									continue;
					}

/*
					tc.widget = eb;
					//eb.reparent(table);
					((Container)table).remove(ph);
					((Container)table).add(eb);
					//table.remove(ph);
					//eb.tableChild = tc;
*/
					//table.remove(ph);
					to_remove.add(ph);
					table.attach (eb, tc.left_attach, tc.right_attach, tc.top_attach, tc.bottom_attach,
													AttachOptions.EXPAND | AttachOptions.SHRINK | AttachOptions.FILL, AttachOptions.EXPAND | AttachOptions.SHRINK | AttachOptions.FILL, tc.xpadding, tc.ypadding);
					this.eventBoxMap.set( eb.box_name, eb );
				}else{
					var eb = tc.widget as OverlayEventBox;
					if( eb != null ){
						//eb.init(); //Könnte später doppelte initialisierung verursachen.
						this.eventBoxMap.set( eb.box_name, eb );
						//eb.tableChild = tc;
						omanager.eventBoxMap.set( eb.box_name, eb );//overwrite if box_name in all loaded ui files not unique
					}
				}


	}

	foreach(var w  in to_remove ){
		table.remove(w);
	}
  table.show_all();
}

/*
 Tausche Platzhalder im Grid gegen die echten Eventboxen aus.
 Achtung, in Gtk.Table gibt es zwei Listenstrukturen. Eine durch Collection, die
 die Unterwidges enthält und eine für "TableChild" (Gtk.List), die die Positionen
 der Unterwidges enthält.

private void cb_replace_placeholders (Widget widget){
				if( !(widget is PlaceholderEventBox) ) return;
				var ph = widget as PlaceholderEventBox;
				var eb = omanager.eventBoxMap.get(ph.box_name);
				var table = ph.get_parent();

				//ph.get_parent().widget = eb;
				//ph.unparent();
				//eb.set_parent(eb);

}*/
/*
private void cb_eventbox (Widget widget){
				if( !(widget is OverlayEventBox) ) return;
				var eb = widget as OverlayEventBox;

				// Add box to eventBoxMap of this window
				this.eventBoxMap.set( eb.box_name, eb );

				eb.foreach(cb_scale_image); //Schlecht, falls Box noch an anderer Stelle eingeblendet ist. 
}

private void cb_scale_image (Widget widget) {
				if( !(widget is Gtk.Image) ) return;
				var image = widget as  Gtk.Image; 
				int dim = (int) (GLib.Math.fmin( width/rows, height/cols) * button_scale);
				image.set_from_pixbuf( 
					image.get_pixbuf().scale_simple( dim, dim, Gdk.InterpType.BILINEAR)
				);

}*/

public override void show(){
	this.is_show = true;
	
	if( this.maximizeOverlay )	this.maximize();
	else{
		corner_move(this.prefered_position);//numpad-coordinates
	}

	base.show();
	present();
//	omanager.loclient.broadcastChangeOverlay( ); //not here... (too many calls)
}

public override void hide(){
	this.is_show = false;

	base.hide();
//	omanager.loclient.broadcastChangeOverlay( );//not here...
}

public void toggle_show()
{
    debug("Toggle show of window");
		if( this.is_show ){
			this.hide();
		}else{
			this.show();
		}
}


public void init_children(Container c){
 if( c is Initable ) ((Initable)c).init();
 //c.foreach( init_cb ); //zu viel des guten... (dopplungen)
}

public void init_cb( Widget widget){
				if( ! ( widget is Container )){
								if( widget is Initable ) ((Initable)widget).init();
				}else{
								this.init_children( (Container) widget);
				}
}

public void set_cell_indizes( Widget w){
	Gtk.Table table;
	if(	( table = this.get_child() as Gtk.Table ) != null ){
		foreach(unowned Gtk.TableChild tc  in table.children ){
		//stdout.printf(@"Tablechild properties: $(tc.left_attach), $(tc.right_attach), $(tc.top_attach), $(tc.bottom_attach), $(tc.xpadding), $(tc.ypadding)\n");
			if( tc.widget == w){
					this.row_active = tc.top_attach;
					this.column_active = tc.left_attach;
					//debug(@"Set cell: ($(this.row_active),$(this.column_active))");
					return;
			}
		}
	}else{
		debug("Window not contain table widget");
	}
}


public Widget? set_cell( int row, int column){

					Gtk.Table table;
					if ( ( table = this.get_child() as Gtk.Table ) != null ){
							//find widget on this position in the (unique) table
							row = (int) ( (row>=this.n_rows)?this.n_rows-1:((row<0)?0:row) );
							column = (int) ( (column>=this.n_columns)?this.n_columns-1:((column<0)?0:column) );

							foreach(unowned Gtk.TableChild tc  in table.children ){
									tc.widget.set_state(Gtk.StateType.NORMAL);
							}
							foreach(unowned Gtk.TableChild tc  in table.children ){
								if( tc.top_attach <= row && row < tc.bottom_attach &&
										tc.left_attach <= column && column < tc.right_attach ){
									this.row_active = row;
									this.column_active = column;
									if( tc.widget != null )
										tc.widget.set_state(Gtk.StateType.PRELIGHT);
										return tc.widget;
								}

							}
						}
						return null;
}

public Widget? set_named_cell( string cellname){

					Widget ret = this.eventBoxMap.get(cellname);
					this.set_cell_indizes( ret );
					return ret;
}

private void corner_move(int pos){
      int screen_width = this.screen.width();
      int screen_height = this.screen.height();
      int x,y,w,h; 
      this.get_size(out w,out h);
     
			x = 0; y = 0; 
      switch (pos){
        /*case 0: //Zur nächsten Position wechseln
          numkeypad_move(this.position_cycle[this.position_num-1]);
          return;*/
        case 7:
          x = 0;
          y = 0;
          break;
        case 8:
          x = (screen_width-w)/2;
          y = 0;
          break;
        case 9:
          x = screen_width-w;
          y = 0;
          break;
        case 4:
          x = 0;
          y = (screen_height-h)/2;
          break;
        case 5:
          y = (screen_height-h)/2;
          break;
        case 6:
          x = screen_width-w;
          y = (screen_height-h)/2;
          break;
        case 1:
          x = 0;
          y = screen_height-h;
          break;
        case 2:
          x = (screen_width-w)/2;
          y = screen_height-h;
          break;
        //case 3: //=default case
        //    ;
        default:
          x = screen_width-w;
          y = screen_height-h;
          break;
        }

      //this.position_num = pos;
      this.move(x,y);
    }



}//End class OverlayWindow


}
