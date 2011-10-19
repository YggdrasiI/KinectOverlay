using GLib;
using X;

namespace Overlay {


	public class Mouse {
		X.Display* display;

		public Mouse(){
			// Open X display
			display = new Display(null);
			if (display == null)
			{
				stderr.printf ("Can't open display!\n");
			}
		}

		~Mouse(){
			//	XCloseDisplay (display);//no, not needed in vala
		}

		public void click(int button){
			//call extern function
			mouse_click( display, button);
		}

	}

}

extern void mouse_click (X.Display* display, int button);

