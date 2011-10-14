/****************************************************************************
 *                                                                           *
 *   Recive OSC messages and connect them with function handler.             *
 *                                                                           *
 *                                                                           *
 *   Author:     Olaf Schulz, 2011                                           *
 *                                                                           *
 ****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <unistd.h>

#include "lo/lo.h"
//#define PORT 7778

//#include "MouseActions.h"

using namespace std;

// ++ Define some handler
extern "C" {
	void error(int num, const char *m, const char *path);

	int generic_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);

	int foo_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);

	int quit_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);

	//int change_overlay_handler(const char *path, const char *types, lo_arg **argv, int argc, void *data, void *user_data);


	// ---

	void error(int num, const char *msg, const char *path)
	{
		printf("liblo server error %d in path %s: %s\n", num, path, msg);
	}

	/* catch any incoming messages and display them. returning 1 means that the
	 * message has not been fully handled and the server should try other methods */
	int generic_handler(const char *path, const char *types, lo_arg **argv,
			int argc, void *data, void *user_data)
	{
		int i;

		printf("path: <%s>\n", path);
		for (i=0; i<argc; i++) {
			printf("arg %d '%c' ", i, types[i]);
			lo_arg_pp( (lo_type) types[i], argv[i]); // cast ok?
			printf("\n");
		}
		printf("\n");
		fflush(stdout);

		return 1;
	}

	int foo_handler(const char *path, const char *types, lo_arg **argv, int argc,
			void *data, void *user_data)
	{
		/* example showing pulling the argument values out of the argv array */
		//printf("%s <- f:%f, i:%d\n\n", path, argv[0]->f, argv[1]->i);
		printf("%s <- s:%s, i:%d\n\n", path, (char*)(&argv[0]->s), argv[1]->i);
		fflush(stdout);

		return 0;
	}

	int quit_handler(const char *path, const char *types, lo_arg **argv, int argc,
			void *data, void *user_data)
	{
		//done = 1;
		// Set done to 1
		*((int*)user_data) = 1;

		printf("quiting\n\n");

		return 0;
	}


}//End Extern
//++++++++++ End handler



class LoServer {
	public:
		LoServer();
		virtual ~LoServer();
		int done;
		lo_server_thread st;

};

LoServer::LoServer(){
	done = 0;

	//extern "C" {
	/* start a new server on port 7770 */
	st = lo_server_thread_new("7778", error);

	/* add method that will match any path and args */
	lo_server_thread_add_method(st, NULL, NULL, generic_handler, NULL);

	/* add method that will match the path /foo/bar, with two numbers, coerced
	 * to float and int */
	//lo_server_thread_add_method(st, "/foo/bar", "fi", foo_handler, NULL);

	/* add method that will match the path /quit with no args */
	//lo_server_thread_add_method(st, "/quit", "", quit_handler, &done);

	lo_server_thread_start(st);
	//}

	//std::cout << "Create ReciveSocket\n";

}

LoServer::~LoServer(){
	//extern "C"{
	lo_server_thread_free(st);
	//}
}



