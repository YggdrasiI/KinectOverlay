//  -*- Mode: Vala; coding: utf-8; indent-tabs-mode: t; tab-width: 8 -*-
/* liblo.vapi
 *
 * Copyright (C) 2011 Artem Popov <artfwo@gmail.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.

 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 *
 */

[CCode (cheader_filename = "lo/lo.h", cprefix = "LO_", lower_case_cprefix = "lo_")]
namespace Lo {
	public const int DEFAULT;
	public const int UDP;
	public const int UNIX;
	public const int TCP;
	
	public const TimeTag TT_IMMEDIATE;
	
	public const int ENOPATH;
	public const int ENOTYPE;
	public const int UNKNOWNPROTO;
	public const int NOPORT;
	public const int TOOBIG;
	public const int INT_ERR;
	public const int EALLOC;
	public const int EINVALIDPATH;
	public const int EINVALIDTYPE;
	public const int EBADTYPE;
	public const int ESIZE;
	public const int EINVALIDARG;
	public const int ETERM;
	public const int EPAD;
	public const int EINVALIDBUND;
	public const int EINVALIDTIME;
	
	[CCode (cname = "lo_err_handler", has_target = false)]
	public delegate void ErrorHandler (int num, string msg, string where);
	
	[CCode (cname = "lo_method_handler", has_target = false)]
	public delegate int MethodHandler (string? path, string? types,
		Arg*[] argv, // no ref or array_length becomes an int*
		Message? msg,
		void* user_data);
	
	[CCode (cname = "lo_bundle_start_handler", has_target = false)]
	public delegate int BundleStartHandler (TimeTag time, void* user_data);
	
	[CCode (cname = "lo_bundle_end_handler", has_target = false)]
	public delegate int BundleEndHandler (void* user_data);
	
	[CCode (cname = "lo_timetag")]
	public struct TimeTag {
		uint32 sec;
		uint32 frac;
	}
	
	[CCode (cprefix = "LO", cname = "lo_type")]
	public enum Type {
		INT32 = 'i',
		FLOAT = 'f',
		STRING = 's',
		BLOB = 'b',
		INT64 = 'h',
		TIMETAG = 't',
		DOUBLE = 'd',
		SYMBOL = 'S', 
		CHAR = 'c',
		MIDI = 'm',
		TRUE = 'T',
		FALSE = 'F', 
		NIL = 'N',
		INFINITUM = 'I';
		[CCode (cname = "lo_is_numerical_type")]
		public int is_numerical ();
		[CCode (cname = "lo_is_string_type")]
		public int is_string ();
	}
	
	// union
	[CCode (cname = "lo_arg")]
	public struct Arg {
		int32 i;
		int32 i32;
		int64 h;
		int64 i64;
		float f;
		float f32;
		double d;
		double f64;
		unowned char s; // to get value use: (string)(&args[i]->s)
		unowned char S; // to get value use: (string)(&args[i]->S)
		uchar c;
		uint8 m[4];
		TimeTag t;
		
		[CCode (cname = "lo_hires_val", instance_pos = -1)]
		public HiRes hires_val (Type type);
		
		[CCode (instance_pos = -1)]
		public void pp (Type type);
	}
	
	[SimpleType]
	[CCode (cname = "lo_hires")]
	public struct HiRes: double {} // FIXME: long double
	
	// cname = "void"
	[Compact]
	[CCode (cname = "void", free_function = "lo_address_free")]
	public class Address {
		public Address (string? host, string port);
		public Address.with_proto (int proto, string host, string port);
		public Address.from_url (string url);
		
		public int errno ();
		public string errstr ();
		public string get_hostname ();
		public string get_port ();
		public int get_protocol ();
		public string get_url ();
		public void set_ttl (int ttl);
		public int get_ttl ();
		public int set_iface (string iface, string ip);
		public string iface ();
	}
	
	// cname = "void"
	[Compact]
	[CCode (cname = "void", free_function = "lo_blob_free")]
	public class Blob {
		public Blob (int32 size, void* data);
		public uint32 datasize ();
		public void* dataptr ();
	}
	
	// cname = "void"
	[Compact]
	[CCode (cname = "void", free_function = "lo_message_free")]
	public class Message {
		public Message ();
		
		[CCode (cname = "lo_send_message", instance_pos = -1)]
		public int send (Address targ, string path);
		
		[CCode (cname = "lo_send_message_from", instance_pos = -1)]
		public int send_from (Address targ, Server serv, string path);
		
		[CCode (sentinel = "")]
		public int add (string types, ...);
		
		public int add_int32 (int32 a);
		public int add_float (float a);
		public int add_string (string a);
		public int add_blob (Blob a);
		public int add_int64 (int64 a);
		public int add_timetag (TimeTag a);
		public int add_double (double a);
		public int add_symbol (string a);
		public int add_char (char a);
		public int add_midi (uint8[] a[4]);
		public int add_true ();
		public int add_false ();
		public int add_nil ();
		public int add_infinitum ();
		
		public Address get_source ();
		public TimeTag get_timestamp ();
		public string get_types ();
		public int get_argc ();
		public Arg*[] get_argv ();
		public size_t length (string path);
		public void* serialise (string path, void* to, out size_t size);
		public static Message deserialise (void* data, size_t size, out int result);
		public void pp ();
	}
	
	// cname = "void"
	// lo_bundle_free_messages not wrapped here
	[Compact]
	[CCode (cname = "void", free_function = "lo_bundle_free")]
	public class Bundle {
		public Bundle (TimeTag tt);
		
		[CCode (cname = "lo_send_bundle", instance_pos = -1)]
		public int send (Address targ);
		
		[CCode (cname = "lo_send_bundle_from", instance_pos = -1)]
		public int send_from (Address targ, Server serv);
		
		public int add_message (string path, Message m);
		public size_t length ();
		public void* serialise (void* to, out size_t size);
		public void pp ();
	}
	
	// cname = "void"
	// no free function in liblo
	[Compact]
	[CCode (cname = "void", free_function = "")]
	public class Method {
		public void pp ();
		public void pp_prefix (string p);
	}
	
	// cname = "void"
	[Compact]
	[CCode (cname = "void", free_function = "lo_server_free")]
	public class Server {
		public Server (string port, ErrorHandler? err_h);
		public Server.with_proto (string port, int proto, ErrorHandler err_h);
		public Server.multicast (string group, string port, ErrorHandler err_h);
		public Server.multicast_iface (string group, string port, string iface, string ip, ErrorHandler err_h);
		public int wait (int timeout);
		public int recv_nonblock (int timeout);
		public int recv ();
		public Method add_method (string? path, string? typespec, MethodHandler h, void* user_data);
		public void del_method (string? path, string? typespec);
		public int add_bundle_handlers (BundleStartHandler sh, BundleEndHandler eh, void* user_data);
		public int get_socket_fd ();
		public int get_port ();
		public int get_protocol ();
		public string get_url ();
		public int events_pending ();
		public double next_event_delay ();
		public int dispatch_data (void* data, size_t size);
		public void pp ();
	}
	
	// cname = "void"
	[Compact]
	[CCode (cname = "void", free_function = "lo_server_thread_free")]
	public class ServerThread {
		public ServerThread (string port,  ErrorHandler? err_h);
		public ServerThread.multicast (string group, string port, ErrorHandler err_h);
		public ServerThread.with_proto (string port, int proto, ErrorHandler err_h);
		public Method add_method (string? path, string? typespec, MethodHandler h, void* user_data);
		public void del_method (string? path, string? typespec);
		public int start ();
		public int stop ();
		public int get_port ();
		public string get_url ();
		public Server get_server ();
		public int events_pending ();
		public void pp ();
	}
	
	[CCode (sentinel = "")]
	public static int send (Address targ, string path, string type, ...);
	
	[CCode (cname = "lo_send_from", sentinel = "")]
	public static int send_from (Address targ, Server from, TimeTag ts, string path, string type, ...);
	
	[CCode (cname = "lo_send_timestamped", sentinel = "")]
	public static int send_timestamped (Address targ, TimeTag ts, string path, string type, ...);
	
	public static string url_get_protocol (string url);
	public static int url_get_protocol_id (string url);
	public static string url_get_hostname (string url);
	public static string url_get_port (string url);
	public static string url_get_path (string url);
	public static int strsize (string s);
	public static int32 blobsize (Blob b);
	public static int pattern_match (string str, string p);
	public static double timetag_diff (TimeTag a, TimeTag b);
	public static void timetag_now (out TimeTag t);
	public static size_t arg_size (Type type, void* data);
	public static size_t get_path (void* data, ssize_t size);
	public static void arg_host_endian (Type type, void* data);
	public static void arg_network_endian (Type type, void* data);
	public static int coerce (Type type_to, Arg* to, Type type_from, Arg* from);
}
