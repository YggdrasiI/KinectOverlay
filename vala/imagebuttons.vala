using Gtk;
using Gdk;
using GLib;
using X;

namespace Overlay {

				public interface Initable /*: Buildable*/{
								public void init(){ } // abstract => virtual => bad
				}

				public interface Copyable {
								public abstract /*virtual*/ OverlayEventBox copy();
				}


				/* Platzhalter-Object beim Laden des Userinterfaces.
					 Wird gegen die Box mit dem Namen box_name nach dem Laden ausgetauscht.
				 */
				public class PlaceholderBox : Gtk.EventBox, Initable {
								public string box_name {get; set; }

								public void init(){
												//debug(@"Placeholder for $(box_name)");
								}

				}

				// Stellt eine on-Clickroutine zur Verfügung, die dem Button eine Funktion zuweist.
				// Z.b den Aufruf einen Shortcuts.
				public class OverlayEventBox : Gtk.EventBox, Initable, Buildable  {

								public new void parser_finished (Builder builder){
												//debug(@"BUILDER paser Farbe null $(this.bg_color_normal.hash()) ");
												/*
													 Gdk.Color c1, c2;
													 if( this.bg_color_normal.hash() == 0 ){
													 Gdk.Color.parse("#0000cc", out c1);
													 this.bg_color_normal =c1;
													 }
													 if( this.bg_color_mouseover.hash() == 0 ){
													 Gdk.Color.parse("#0000ff", out c2);
													 this.bg_color_mouseover = c2;
													 }*/
												init();
								}


								public string box_name {set; get; }
								public Gdk.Color bg_color_normal {set ; get; }
								public Gdk.Color bg_color_mouseover {set ; get; }
								protected bool hide_after_action {get; set; }

								public OverlayEventBox(){
												// Default colors
												/*
													 Gdk.Color c1, c2;
													 Gdk.Color.parse("#0000cc", out c1);
													 Gdk.Color.parse("#0000ff", out c2);
													 this.bg_color_normal =c1;
													 this.bg_color_mouseover = c2;
												 */
												this.hide_after_action = true;

												init();
								}

								public OverlayEventBox.copy(OverlayEventBox orig){
												this.box_name = orig.box_name;
												this.bg_color_normal = orig.bg_color_normal;
												this.bg_color_mouseover = orig.bg_color_mouseover;
												this.hide_after_action = orig.hide_after_action;

												var c = orig.get_child();
												if( c is Gtk.Image ){
																this.add( new Gtk.Image.from_pixbuf( ((Gtk.Image)c).get_pixbuf() ) );
												}

												//debug("CopyBox, before call init()");
												init();
								}

								public virtual OverlayEventBox copyBox(){
												//debug("OverleyEventBox.copyBox()");
												return new OverlayEventBox.copy(this);
								}

								public new void init(){
												//debug("Box initialisiert\n");
												//debug("Init OverlayEventBox");
												//	omanager.eventBoxMap.set(this.box_name,this);//hier falsch
												Gdk.Color c1, c2;
												Gdk.Color.parse("#4444cc", out c1);
												Gdk.Color.parse("#66dd66", out c2);
												this.bg_color_normal =c1;
												this.bg_color_mouseover = c2;

												this.modify_bg(Gtk.StateType.NORMAL, this.bg_color_normal);
												this.modify_bg(Gtk.StateType.PRELIGHT, this.bg_color_mouseover);

												// Default handler
												//	this.set_events(Gdk.EventMask.BUTTON_MOTION_MASK+ Gdk.EventMask.BUTTON_RELEASE_MASK);
												this.button_release_event.connect( my_button_release_event  );
												this.enter_notify_event.connect( my_enter_notify_event );
												this.leave_notify_event.connect( my_leave_notify_event );

												//this.hide_after_action = true;
								}


								public virtual bool my_button_release_event (EventButton source){
												//debug("Button activated");
												omanager.loclient.broadcastCellClicked();
												return false;
								}

								public bool my_enter_notify_event (EventCrossing source){
												//this.modify_bg(Gtk.StateType.NORMAL, this.bg_color_mouseover);
												if( omanager.getActiveOverlay().react_on_mouse){
																this.set_state(Gtk.StateType.PRELIGHT);
																omanager.getActiveOverlay().set_cell_indizes(this);
												}
												return false;
								}
								public bool my_leave_notify_event (EventCrossing source) {
												//this.modify_bg(Gtk.StateType.NORMAL, this.bg_color_normal);
												if( omanager.getActiveOverlay().react_on_mouse){
																this.set_state(Gtk.StateType.NORMAL);
												}
												return false;
								}

								protected ulong find_keysym(string s){
												ulong keysym = string_to_keysym(s);
												if( keysym == 0 ){
																//accelerator_parse fails for many special keysyms, like Delete, Return
																// detect them here manually. (Missing: converter for [keysym_variable_name] => value :-( )
																if ( "<Shift>" in s)	keysym = XK_Shift_L;
																if ( "<Ctrl>" in s)	keysym = XK_Control_L;
																if ( "<Alt>" in s)	keysym = XK_Alt_L;
																if ( "<Delete>" in s)	keysym = XK_Delete;
																//if ( "<Return>" in s)	keysym = XK_Return;
																if ( "<Return>" in s)	keysym = XK_KP_Enter;
																if ( "<BackSpace>" in s)	keysym = XK_BackSpace;
																if ( "<Insert>" in s)	keysym = XK_Insert;
																if ( "<PageUp>" in s)	keysym = XK_Prior;
																if ( "<PageDown>" in s)	keysym = XK_Next;
																if ( "<Up>" in s)	keysym = XK_Up;
																if ( "<Down>" in s)	keysym = XK_Down;
																if ( "<Left>" in s)	keysym = XK_Left;
																if ( "<Right>" in s)	keysym = XK_Right;
																if ( "<Space>" in s)	keysym = XK_space;
																if ( "<XK_KP_1>" in s)	keysym = XK_KP_1;
																if ( "<XK_KP_2>" in s)	keysym = XK_KP_2;
																if ( "<XK_KP_3>" in s)	keysym = XK_KP_3;
																if ( "<XK_KP_4>" in s)	keysym = XK_KP_4;
																if ( "<XK_KP_5>" in s)	keysym = XK_KP_5;
																if ( "<XK_KP_6>" in s)	keysym = XK_KP_6;
																if ( "<XK_KP_7>" in s)	keysym = XK_KP_7;
																if ( "<XK_KP_8>" in s)	keysym = XK_KP_8;
																if ( "<XK_KP_9>" in s)	keysym = XK_KP_9;
																if ( "<XK_KP_0>" in s)	keysym = XK_KP_0;
																if ( "<F1>" in s)	keysym = XK_F1;
																if ( "<F2>" in s)	keysym = XK_F2;
																if ( "<F3>" in s)	keysym = XK_F3;
																if ( "<F4>" in s)	keysym = XK_F4;
																if ( "<F5>" in s)	keysym = XK_F5;
																if ( "<F6>" in s)	keysym = XK_F6;
																if ( "<F7>" in s)	keysym = XK_F7;
																if ( "<F8>" in s)	keysym = XK_F8;
																if ( "<F9>" in s)	keysym = XK_F9;
																if ( "<F10>" in s)	keysym = XK_F10;
																if ( "<F11>" in s)	keysym = XK_F11;
																if ( "<F12>" in s)	keysym = XK_F12;
																if ( "<Tab>" in s)	keysym = XK_Tab;
																if ( "<Period>" in s)	keysym = XK_period;
																if ( "<Comma>" in s)	keysym = XK_comma;
												}
												//		debug(@"$s: $keysym");
												return keysym;
								}

				}

				public struct keymod{
								public uint keysym;
								public uint modsym1;
								public uint modsym2;
								public keymod(uint keysym, uint modsym1, uint modsym2){
												this.keysym = keysym;
												this.modsym1 = modsym1;
												this.modsym2 = modsym2;
								}
				}

				public class ShortcutEventBox : OverlayEventBox, Initable, Buildable  {

								public new void parser_finished (Builder builder){
												//debug("BUILDER paser shortcut");
												base.parser_finished(builder);
												init();
								}

								public string shortcut {get; set;}
								//public uint[] keysym {get; set; }
								//public int[]  modifiers {get; set; }
								public keymod[] keymods {get; set; }

								public ShortcutEventBox(){
												base();
								}

								public ShortcutEventBox.copy(ShortcutEventBox orig){
												base.copy(orig);
												this.shortcut = orig.shortcut;
												this.keymods = orig.keymods;
												init();
								}

								public override OverlayEventBox copyBox(){
												//debug("ShortcutEventBox.copyBox()");
												return new ShortcutEventBox.copy(this);
								}

								public new void init(){
												//base.init();
												//debug("Init ShortcutEventBox");
												uint keysym, modsym1, modsym2;
												//debug(@"$(this.shortcut)\n");
												//var commaspit = new Regex("(#[^=]*)*[ ]*=[ ]*");
												var x = this.shortcut.split(",");
												//this.keysym = new uint[x.length];
												//this.modifiers = new int[x.length];
												this.keymods = new keymod[x.length];
												int index = 0;
												foreach( var s in x){
																var xx = s.split("+");
																int l = xx.length;
																keysym = (uint) find_keysym( xx[l-1] );
																if( l>1 )
																				modsym1 = (uint) find_keysym( xx[l-2] );
																else
																				modsym1 = 0;
																if( l>2 )
																				modsym2 = (uint) find_keysym( xx[l-3] );
																else
																				modsym2 = 0;

																debug(@"Shortcutteil: $s, %u, %u, %u",  keysym, modsym1, modsym2);

																if( keysym != 0 ){
																				this.keymods[index] = keymod(keysym, modsym1, modsym2);
																				index++;
																}
												}

												this.keymods = this.keymods[0:index];
								}


								public override bool my_button_release_event (EventButton source){
												//debug("Shortcutbutton activated.");

												//omanager.hideOverlay();
												this.send_shortcut();
												omanager.loclient.broadcastCellClicked();
												return false;
								}

								public void send_shortcut(){
												stdout.printf("Key command length: %i\n", this.keymods.length);
												int i=0;
												foreach( var x in this.keymods){
																stdout.printf("Send key. %u, %u, %u\n", x.keysym, x.modsym1, x.modsym2);
																keysend2( x.keysym, x.modsym1, x.modsym2 );
																i++;
																if( i < this.keymods.length)
																				Posix.sleep(1);
												}

												//hide overlay
												if( hide_after_action ){
																omanager.hideActiveOverlay();
												}

								}

				}

				public class ConfirmedShortcut : ShortcutEventBox {

								private OverlayWindow sourceWin;

								public ConfirmedShortcut.copy(ConfirmedShortcut orig){
												base.copy(orig);
												//init();//no init in this class, not run base.init twice
								}

								public override OverlayEventBox copyBox(){
												//debug("ConfirmedShortcut.copyBox()");
												return new ConfirmedShortcut.copy(this);
								}

								public override bool my_button_release_event (EventButton source){
												//debug("YesNobutton activated.");

												OverlayWindow yesNo = omanager.getOverlay("YesNo.ui");
												var yes = yesNo.eventBoxMap.get("yes") as SignalEventBox;
												SignalEventBox no = (SignalEventBox) yesNo.eventBoxMap.get("no");

												//int confirm = -1;
												/*
													 if( handler_id_yes == -1)
													 handler_id_yes = yes.sig_button_release.connect( (ebox, button)  => { this.confirm(false);} );
													 if( handler_id_no == -1)
													 handler_id_no = no.sig_button_release.connect( (ebox, button)  => { this.confirm(true);} );
												 */
												this.sourceWin = omanager.getActiveOverlay();
												yes.release_action = (button) => { this.confirm(true, button ); };
												no.release_action = (button) => { this.confirm(false, button ); };

												omanager.showOverlay("YesNo.ui");

												return false;
								}

								public void confirm(bool yes, EventButton source){

												omanager.hideOverlay("YesNo.ui");
												//omanager.hideActiveOverlay();
												omanager.setActiveOverlay( this.sourceWin );

												if( yes ){
																//debug("Send confirmed key");
																base.my_button_release_event ( source );
												}

												//	omanager.loclient.broadcastCellClicked();//send in base....()

								}


				}

				public class ShowWindowEventBox : OverlayEventBox, Initable, Buildable {

								public new void parser_finished (Builder builder){
												//debug("BUILDER paser ShowWindow");
												base.parser_finished(builder);
												init();
								}

								public string uiname {get; set; }
								public int mode {get; set; }

								public ShowWindowEventBox(){
												base();
								}

								public ShowWindowEventBox.copy(ShowWindowEventBox orig){
												base.copy(orig);
												this.uiname = orig.uiname;
												this.mode = orig.mode;
												init();
								}

								public override OverlayEventBox copyBox(){
												//debug("ShowWindow.copyBox()");
												return new ShowWindowEventBox.copy(this);
								}


								public new void init(){
												//debug("Init ShowWindowEventBox");
								}

								public override bool my_button_release_event ( EventButton source){
												//debug("ShowWindowButton activated");
												//send before active window switch
												omanager.loclient.broadcastCellClicked();

												if( this.mode == 0)
																omanager.hideOverlay(this.uiname);
												if( this.mode == 1)
																omanager.showOverlay(this.uiname);
												if( this.mode == 2)
																omanager.toggleOverlay(this.uiname);

												return false;
								}
				}

				public class HideWindowEventBox: OverlayEventBox{

								public OverlayWindow winMain {get; set; }

								public HideWindowEventBox(){
												base();
								}

								public HideWindowEventBox.copy(HideWindowEventBox orig){
												base.copy(orig);
												this.winMain = orig.winMain;
								}

								public override OverlayEventBox copyBox(){
												//debug("HideWindow.copyBox()");
												return new HideWindowEventBox.copy(this);
								}

								public override bool my_button_release_event ( EventButton source){

												//winMain.hide();
												omanager.loclient.broadcastCellClicked();
												omanager.hideActiveOverlay();

												return false;
								}

				}


				public delegate void ReleaseEventType(EventButton source);
				/* With signal(s) for events */
				public class SignalEventBox: OverlayEventBox, Initable, Buildable {

								public new void parser_finished (Builder builder){
												//debug("BUILDER paser Signal");
												base.parser_finished(builder);
												init();
								}

								//public signal void sig_button_release( EventButton source);
								public ReleaseEventType release_action = null;

								public SignalEventBox(){
												base();
								}

								public SignalEventBox.copy(SignalEventBox orig){
												base.copy(orig);
												this.release_action = orig.release_action;
								}

								public override OverlayEventBox copyBox(){
												//debug("SignalWindow.copyBox()");
												return new SignalEventBox.copy(this);
								}

								public new void init(){
												//base.init();
												//debug("Init SignalEventBox");
												//sig_button_release.connect( (t, button) => {stdout.printf("Signal!"); } );
								}

								public override bool my_button_release_event (EventButton source){
												//sig_button_release(source);
												if( release_action != null )
																release_action(source);

												//no broadcast here

												return false;
								}

				}

				/* Combine ShortcutEventBox and ShowWindowEventBox
				 */
				public class ShortcutAndShowWindow : ShortcutEventBox, Initable, Buildable  {

								public new void parser_finished (Builder builder){
												base.parser_finished(builder);
												init();
								}

								public string uiname {get; set; }
								public int mode {get; set; }

								public ShortcutAndShowWindow(){
												base();
								}

								public ShortcutAndShowWindow.copy(ShortcutAndShowWindow orig){
												base.copy(orig);
												this.uiname = orig.uiname;
												this.mode = orig.mode;
												init();
								}

								public override OverlayEventBox copyBox(){
												//debug("ShortcutEventBox.copyBox()");
												return new ShortcutAndShowWindow.copy(this);
								}

								public new void init(){
												// nothing
								}


								public override bool my_button_release_event (EventButton source){
												//debug("Shortcutbutton activated.");

												//omanager.hideOverlay();
												this.send_shortcut();
												omanager.loclient.broadcastCellClicked();

												if( this.mode == 0)
																omanager.hideOverlay(this.uiname);
												if( this.mode == 1)
																omanager.showOverlay(this.uiname);
												if( this.mode == 2)
																omanager.toggleOverlay(this.uiname);

												return false;
								}

				}

				/* Class for sending Osc Events */
				public class SendOscMessageBox : OverlayEventBox, Initable, Buildable {

								public new void parser_finished (Builder builder){
												base.parser_finished(builder);
												init();
								}

								public string messagepath {get; set; }
								public string messagename {get; set; }
								public int messagevalue {get; set; }

								public SendOscMessageBox(){
												base();
								}

								public SendOscMessageBox.copy(SendOscMessageBox orig){
												base.copy(orig);
												this.messagepath = orig.messagepath;
												this.messagename = orig.messagename;
												this.messagevalue = orig.messagevalue;
												init();
								}

								public override OverlayEventBox copyBox(){
												return new SendOscMessageBox.copy(this);
								}


								public new void init(){
								}

								public override bool my_button_release_event ( EventButton source){
												//send before active window switch
												stdout.printf("General message\n");
												if( hide_after_action ){
																omanager.hideActiveOverlay();
												}
												omanager.loclient.broadcastStringMessage(this.messagepath, this.messagename, this.messagevalue);
												return false;
								}
				}


}//End Namespace
