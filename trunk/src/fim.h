/* $Id$ */
/*
 fim.h : Fim main header file

 (c) 2007-2009 Michele Martone

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
*/
#ifndef FIM_FIM_H
#define FIM_FIM_H
#define NDEBUG 1	/* turns off assert() functionality */


/*
 * config.h is generated by autoconf and defines some symbols used by Fim
 * */
#ifdef HAVE_CONFIG_H
/*
 * when we get rid of string.h we will use <config.h>, too
#include <config.h>
*/
#include "../config.h"
#endif

/*
 *	This is the main fim program header file.
 *	Estabilished 20061225 ( yes, i know what do you think about me now )
 * */
#include <cstdlib>	/* <stdlib.h> standard C library definitions			*/
#include <cstdio>	/* <stdlib.h> standard C buffered I/O 				*/

#include <cstring>	/* <string.h> standard C string manipulation functions		*/
#include <iostream>	/* C++ I/O stream manipulation ( cout,cin, ... )		*/
#include <algorithm>	/* STL (Standard Template Library) algorithm library		*/
#include <map>		/* STL (Standard Template Library) associative array template	*/
#include <stack>	/* STL (Standard Template Library) stack structure template	*/

#include <vector>	/* STL (Standard Template Library) vector structure template	*/
#include <utility>	/* STL (Standard Template Library) ?				*/
#include <list> 	/* STL (Standard Template Library) list structure template	*/
#include <set> 		/* STL (Standard Template Library) set structure template 	*/
#include <cassert>	/* <assert.h> C assertions ( IEEE Std 1003.1-2001 aka Posix ) 	*/

#ifndef USE_GNU_REGEX
# include <regex.h>		/*	the Posix (GNU implementation,not functionality) readline library	*/
#else
# include "regex.h"		/*	the GNU (implementation and functionality) readline library	*/
#endif

#ifdef HAVE_CLIMITS
 #include <climits>
 /* From:
  * ISO C++ 14882: 18.2.2  Implementation properties: C library 
  * we get some more stuff than limits.h. */
#else
 #ifdef HAVE_LIMITS_H
 /* According to IEEE Std 1003.1-2001, 
  * this should define _POSIX_PATH_MAX
  * */
  #include <limits.h>
 #endif
#endif
#ifndef _POSIX_PATH_MAX
 /* I don't know in what case could arise this situation 
  * (no limits.h nor climits : i guess a badly configured system),
  * but this would be the fix :*/
 #define _POSIX_PATH_MAX 4096
#endif

#if 0
#ifdef HAVE_UNIX_H
# include <unix.h>
# ifndef _POSIX_PATH_MAX
#  include <posix1_lim.h>
#  ifndef _POSIX_PATH_MAX
/*  a strict limit to pathname length */
#   define _POSIX_PATH_MAX 4096
#  endif
# endif
#else
#endif
#endif

#include <termios.h>	/* general terminal interface (Posix)			*/
#include <fcntl.h>	/* file descriptor manipulation interface (Posix)	*/
#include <time.h>	/* time related functionality (Posix)			*/
#include "common.h"	/* misc FIM stuff					*/
#ifdef FIM_USE_GPM
#include <gpm.h>	/* mouse events						*/
#endif
/*#include <unistd.h>*/ /* standard Posix symbolic constants and types		*/
/*#include <sys/stat.h> */
/*#include <sys/types.h>*/
 
extern int g_fim_no_framebuffer;

namespace fim
{
	typedef std::map<fim::string,int > key_bindings_t;	//symbol->code
	void status(const char *desc, const char *info);

	class Arg;
	class Browser;
	class FontServer;
	class FramebufferDevice;
	class CommandConsole;
	class Command;
	class Foo;
	class Image;
	class Var;
	class MiniConsole;
#ifdef FIM_WINDOWS
	class Window;
#endif
	class Viewport;
	class fim_stream;
}

namespace rl
{
	void initialize_readline (int with_no_display_device);
}

/* using GCC builtins */

#ifndef FIM_IS_SLOWER_THAN_FBI
	#define FIM_UNLIKELY(expr) __builtin_expect(!!(expr),0)
	#define FIM_LIKELY(expr)   __builtin_expect(!!(expr),1)
	#define FIM_ALIGNED __attribute__((aligned (64)))
#else
	#define FIM_UNLIKELY(expr)  (expr)
	#define FIM_LIKELY(expr)   (expr)
	#define FIM_ALIGNED
#endif

//#define FIM_FBI_PRINTF( ... ) fprintf(stderr, __VA_ARGS__ )
/* " warning: anonymous variadic macros were introduced in C99" (here and elsewhere) */
#define FIM_FBI_PRINTF( ... ) 1

namespace fim{
enum fim_image_source_t { FIM_E_FILE=-11, FIM_E_STDIN=-22};	/* these */
typedef std::pair<fim::string,fim_image_source_t > 	   cache_key_t;	//the current cache key
};
typedef std::vector<fim::string> args_t;
#define FIM_STDIN_IMAGE_NAME "<STDIN>"
/* should belong to a namespace different from the file name space, and possibly figuring alphabetically as the first one */

#include "string.h"
#include "Command.h"
#include "Arg.h"
#include "FontServer.h"
#include "FbiStuff.h"
#include "Var.h"
#include "Namespace.h"
#include "Image.h"
#include "Cache.h"
#include "Viewport.h"
#include "Window.h"
#include "Browser.h"
#include "DebugConsole.h"
#include "DummyDisplayDevice.h"
#ifdef FIM_WITH_LIBSDL
#include "SDLDevice.h"
#endif
#ifdef FIM_WITH_CACALIB
#include "CACADevice.h"
#endif
#ifdef FIM_WITH_AALIB
#include "AADevice.h"
#endif
#include "FramebufferDevice.h"
#include "CommandConsole.h"
#include "fim_stream.h"

namespace fim
{
	class Browser;
	class CommandConsole;

	static fim_stream cout;	// this will be moved
	std::ostream& operator<<(std::ostream &os,const string& s);
}

/*
 * Fim language variable identifiers.
 * */
#define FIM_VID_BINARY_DISPLAY 			"_display_as_binary"
#define FIM_VID_BINARY_DISPLAY_BPP 		"_display_as_binary_bpp"
#define FIM_VID_CACHE_STATUS 			"_cache_status"
#define FIM_VID_DISPLAY_CONSOLE 		"_display_console"
#define FIM_VID_DEVICE_DRIVER 			"_device_driver"
#define FIM_VID_DISPLAY_STATUS			"_display_status"
#define FIM_VID_LOAD_DEFAULT_ETC_FIMRC 		"_load_default_etc_fimrc"
#define FIM_VID_NO_RC_FILE			"_no_rc_file"
#define FIM_VID_SCRIPTOUT_FILE			"_fim_scriptout_file"
#define FIM_VID_STATUS_LINE 			"_status_line"
#define FIM_VID_WANT_PREFETCH 			"_want_prefetch"
#define FIM_VID_AUTO_SCALE_V			"auto_scale_v"	/* fixme : should be moved to fimrc's scope only */
#define FIM_VID_AUTOTOP				"autotop"
#define FIM_VID_AUTOWIDTH			"autowidth"
#define FIM_VID_FILEINDEX			"fileindex"
#define FIM_VID_FILELISTLEN			"filelistlen"
#define FIM_VID_FILENAME			"filename"
#define FIM_VID_FIM_DEFAULT_CONFIG_FILE_CONTENTS "FIM_DEFAULT_CONFIG_FILE_CONTENTS"
#define FIM_VID_FRESH				"fresh"
#define FIM_VID_OVERRIDE_DISPLAY		"_override_display"
#define FIM_VID_MAX_ITERATED_COMMANDS		"_max_iterated_commands"
#define FIM_VID_MAGNIFY_FACTOR			"magnify_factor"
#define FIM_VID_PWD				"pwd"
#define FIM_VID_REDUCE_FACTOR			"reduce_factor"
#define FIM_VID_SCALE_FACTOR_MULTIPLIER		"scale_factor_multiplier"
#define FIM_VID_SCALE_FACTOR_DELTA		"scale_factor_delta"
#define FIM_VID_STEPS 				"steps"
#define FIM_VID_CONSOLE_ROWS 			"rows"
#define FIM_VID_CONSOLE_LINE_WIDTH 		"lwidth"
#define FIM_VID_CONSOLE_LINE_OFFSET 		"console_offset"
#define FIM_VID_CONSOLE_BUFFER_LINES		"console_lines"
#define FIM_VID_CONSOLE_BUFFER_TOTAL		"console_buffer_total"
#define FIM_VID_CONSOLE_BUFFER_FREE		"console_buffer_free"
#define FIM_VID_CONSOLE_BUFFER_USED		"console_buffer_used"
#define FIM_VID_VERBOSE_KEYS			"_verbose_keys"
#define FIM_VID_CONSOLE_KEY			"console_key"
#define FIM_VID_IGNORECASE			"ignorecase"
#define FIM_VID_SAVE_FIM_HISTORY		"_save_fim_history"
#define FIM_VID_LOAD_FIM_HISTORY		"_load_fim_history"
#define FIM_VID_TERM				"_TERM"
#define FIM_VID_NO_DEFAULT_CONFIGURATION	"_no_default_configuration"
#define FIM_VID_DISPLAY_STATUS_BAR		"_display_status_bar"
#define FIM_VID_DISPLAY_BUSY			"_display_busy"
#define FIM_VID_SCALE				"scale"
#define FIM_VID_ASCALE				"ascale"
#define FIM_VID_ANGLE				"angle"
#define FIM_VID_ORIENTATION			"orientation"
#define FIM_VID_WIDTH				"width"
#define FIM_VID_SWIDTH				"swidth"
#define FIM_VID_SHEIGHT				"sheight"
#define FIM_VID_HEIGHT				"height"
#define FIM_VID_AUTOFLIP			"autoflip"
#define FIM_VID_FLIPPED				"flipped"
#define FIM_VID_FIM_BPP				"_fim_bpp"
#define FIM_VID_AUTOMIRROR			"automirror"
#define FIM_VID_MIRRORED			"mirrored"
#define FIM_VID_WANT_AUTOCENTER			"want_autocenter"
#define FIM_VID_MAX_CACHED_IMAGES		"_max_cached_images"
#define FIM_VID_MAX_CACHED_MEMORY		"_max_cached_memory"
#define FIM_VID_CACHED_IMAGES			"_cached_images"

/*
 * Help messages for Fim commands (partial).
 * One glorious day these macros will serve to build automatically documentation.
 * */
#define FIM_CMD_HELP_CD			"cd {path}: change the current directory to {path}. cd - will change to the previous current directory (before the last \":cd {path} command\")"
#define FIM_CMD_PWD_CD			"print the current directory name."
#define FIM_CMD_EVAL_CD			"evaluates the arguments as commands, executing them."
#define FIM_CMD_SYSTEM_CD		"system {expr}: get the output of the shell command {expr}."

/*
 * Some Fim internals flags.
 */
#define FIM_FLAG_MIRROR 1
#define FIM_FLAG_FLIP 2
#define FIM_FLAG_RGB2GRAY 4
#define FIM_FLAG_RGB2GRAYGRAYGRAY 8

#endif
