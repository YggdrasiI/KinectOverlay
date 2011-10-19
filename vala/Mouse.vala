using GLib;
using X;

namespace Overlay {

	X.Display* display;

	public class Mouse{

		public Mouse(){
			// Open X display
			Display *display = new Display(null);
			if (display == null)
			{
				stderr.fprintf ("Can't open display!\n");
			}
		}

		~Mouse(){
			//	XCloseDisplay (display);//no, not needed in vala
		}

		void click(int button){
			//call extern function
			click( display, button);
		}

	}

}

extern void click (X.Display *display, int button)

