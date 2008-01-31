/* $Id$ */
/*
 fim.h : Fim main header file

 (c) 2007-2008 Michele Martone

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
#ifndef FIM_H
#define FIM_H
#define NDEBUG 1	/* turns off assert() functionality */


/*
 * config.h is generated by autoconf and defines some symbols used by Fim
 * */
#include "../config.h"

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

#ifdef HAVE_UNIX_H
# include <unix.h>
# ifndef _POSIX_PATH_MAX
#  include <posix1_lim.h>
#  ifndef _POSIX_PATH_MAX
/*  a strict limit to pathname length */
#   define _POSIX_PATH_MAX 4096
#  endif
# endif
#endif

//
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
 
namespace fim{
	void tty_restore();
	void tty_raw(void);
	void cleanup_and_exit(int code);
	void status(const char *desc, const char *info);

void status_screen(const char *desc, char *info);
void fb_status_screen(const char *msg, int noDraw);
}

namespace fim{
class Arg;
class Browser;
class CommandConsole;
class FontServer;
class FramebufferDevice;
class Command;
class Image;
class Var;
#ifdef FIM_WINDOWS
class Window;
class Viewport;
#endif
class fim_stream;
}

namespace rl{
//char ** fim_completion (const char *text, int start,int end);
//void completion_display_matches_hook(char **matches,int num,int max);
//void redisplay();
//int redisplay_hook();
void initialize_readline ();
}
#include "extern.h"
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
#include "CommandConsole.h"
#include "FramebufferDevice.h"
#include "fim_stream.h"

char * dupstr (const char* s);
char * dupnstr (double n);
char * dupnstr (int n);

namespace fim
{
static fim_stream cout;
class Browser;
class CommandConsole;
std::ostream& operator<<(std::ostream &os,const string& s);
//void status(const char *desc, const char *info);
void set_status_bar(const char *desc, const char *info);
void set_status_bar(fim::string desc, const char *info);
}

int help_and_exit(char *argv0);
int fim_rand();
namespace rl
{
	//int fim_set_command_line_text(const char*s);
}

/*
 * Fim language variable names.
 * */
#define FV__CACHE_STATUS 		"_cache_status"
#define FV__DISPLAY_CONSOLE 		"_display_console"
#define FV__DISPLAY_STATUS		"_display_status"
#define FV__LOAD_DEFAULT_ETC_FIMRC 	"_load_default_etc_fimrc"
#define FV__NO_RC_FILE			"_no_rc_file"
#define FV__STATUS_LINE 		"_status_line"
#define FV_AUTO_SCALE_V			"auto_scale_v"
#define FV_AUTOTOP			"autotop"
#define FV_AUTOWIDTH			"autowidth"
#define FV_FILEINDEX			"fileindex"
#define FV_FILELISTLEN			"filelistlen"
#define FV_FILENAME			"filename"
#define FV_FIM_DEFAULT_CONFIG_FILE_CONTENTS "FIM_DEFAULT_CONFIG_FILE_CONTENTS"
#define FV_FRESH			"fresh"
#define FV_MAGNIFY_FACTOR		"magnify_factor"
#define FV_PWD				"pwd"
#define FV_REDUCE_FACTOR		"reduce_factor"
#define FV_STEPS 			"steps"


#define TRUE            1
#define FALSE           0

#endif
