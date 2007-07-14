/* $Id$ */
/*
 fim.h : Fim main header file

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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#ifndef FBVI_H
#define FBVI_H
#define NDEBUG 1	/* turns off assert() functionality */
/*
 *	This is the main fim program file.
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

#include <readline/readline.h>	/*	the GNU readline library	*/
#include <readline/history.h> 	/*	the GNU readline library	*/
#include <regex.h>		/*	the Posix (GNU implementation,not functionality) readline library	*/

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
}

void status_screen(const char *desc, char *info);
void fb_status_screen(const char *msg, int noDraw);

namespace fim{
class Arg;
class Browser;
class CommandConsole;
class Command;
class Image;
class Var;
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
#include "Var.h"
#include "Cache.h"
#include "Image.h"
#include "Browser.h"
#include "CommandConsole.h"
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
}

//void status(const char *desc, const char *info);
void set_status_bar(const char *desc, const char *info);
void fb_status_screen(const char *msg, int noDraw);
int fim_rand();
namespace rl
{
int fim_set_command_line_text(const char*s);
}
#endif
