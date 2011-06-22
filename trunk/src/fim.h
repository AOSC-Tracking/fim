/* $LastChangedDate$ */
/*
 fim.h : Fim main header file

 (c) 2007-2011 Michele Martone

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

/* FIXME : should create some fim specific sys.h header, some day */
#include <sys/types.h>		/* stat */
#include <sys/stat.h>		/* stat */
#include <unistd.h>		/* stat */
#ifdef FIM_WITH_PTHREADS
#include <pthread.h>		/* */
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
#define FIM_PATH_MAX _POSIX_PATH_MAX

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
 
#define FIM_LINE_CERR std::cerr << "fatal error" << __FILE__ << ":" << __LINE__ << "\n";
#define FIM_LINE_COUT std::cout << "in " <<__func__ << " # " << __FILE__ << ":" << __LINE__ << "\n";

namespace fim
{
	typedef int fim_pan_t;		/* a type for pixel offsets (neg/pos)  */
	typedef int fim_off_t;		/* a type for pixel offsets (positive)  */
	typedef float fim_scale_t;	/* a type for image scaling */
	typedef float fim_angle_t;	/* a type for angles */
	typedef int   fim_page_t;	/* a type for multipage document pages */
	typedef int   fim_pgor_t;	/* a type for page orientation */
	typedef bool   fim_bool_t;	/* a type for bolean expressions */
	typedef int fim_coo_t;		/* a type for coordinates */
	typedef int fim_cc_t;		/* a type for console control */
	typedef int fim_flags_t;	/* a type for display flags */
	typedef int fim_bpp_t;		/* a type for bits Per Pixel */
	typedef int fim_key_t;		/* a type for keycodes */
	typedef int fim_err_t;		/* a type for errors */
	typedef int fim_status_t;	/* a type for fim's status */
	typedef int fim_cycles_t;	/* a type for fim's cycles */
	typedef int fim_cmd_type_t;	/* a type for fim's command types */
	typedef int fim_var_t;		/* a type for fim's variable types */
	typedef int fim_int;		/* a type for fim's internal integer type (TODO: shall extend its use!) */

	typedef int fim_ts_t;		/* a type for time, in seconds */
	typedef int fim_tms_t;		/* a type for time, in milliseconds */
	typedef unsigned long fim_tus_t;	/* a type for time, in microseconds */
	typedef std::map<fim::string,fim_key_t > sym_keys_t;	//symbol->code
	typedef char fim_char_t;	/* a type for chars */
	typedef unsigned char fim_byte_t;	/* a type for bytes */

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
	void initialize_readline (fim_bool_t with_no_display_device);
}

/* using GCC builtins (__GNUC__ should be defined by gcc) */

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
enum fim_xflags_t { FIM_X_NULL=0,FIM_X_HISTORY=1,FIM_X_QUIET=2,FIM_X_NOAUTOCMD=4};	/* TODO: may introduce 'sandbox' like flags, here (for instance, for no-system/pipe-interacting executions) */
typedef std::pair<fim::string,fim_image_source_t > 	   cache_key_t;	//the current cache key
}
typedef std::vector<fim::string> args_t;
#define FIM_STDIN_IMAGE_NAME "<STDIN>"
/* should belong to a namespace different from the file name space, and possibly figuring alphabetically as the first one */

enum FimDocRefMode{ Txt, Man, DefRefMode=Txt};

/*
 * Fim Symbols
 * */
#define FIM_SYM_CONSOLE_KEY	':'
#define FIM_SYM_CONSOLE_KEY_STR	":"
#define FIM_SYM_SEARCH_KEY	'/'
#define FIM_SYM_NULL_KEY	0
#define FIM_SYM_NAMESPACE_SEP	':'
#define FIM_SYM_DEVOPTS_SEP	'='
#define FIM_SYM_DEVOPTS_SEP_STR	"="
#define FIM_SYM_NULL_NAMESPACE_CHAR	'\0'
#define FIM_SYM_NAMESPACE_BROWSER_CHAR	'b'
#define FIM_SYM_NAMESPACE_IMAGE_CHAR	'i'
#define FIM_SYM_NAMESPACE_GLOBAL_CHAR	'g'
#define FIM_SYM_NAMESPACE_WINDOW_CHAR	'w'
#define FIM_SYM_NAMESPACE_VIEWPORT_CHAR	'v'
#define FIM_SYM_NAMESPACE_PREFIXES	"'i:', 'b:', 'w:', 'v:', 'g:'"
#define FIM_SYM_NAMESPACE_PREFIXES_DSC	"current image, browser, window, viewport, global"
#define FIM_SYM_NAMESPACE_REGEX	"^[givbw]:"
#define FIM_SYM_PROMPT_CHAR	':'
#define FIM_SYM_PROMPT_SLASH	'/'
#define FIM_SYM_PROMPT_NUL	'\0'
#define FIM_SYM_CHAR_NUL	'\0'
#define FIM_SYM_FLIPCHAR	'F'
#define FIM_SYM_MIRRCHAR	'M'
#define FIM_SYM_TYPE_FLOAT	'f'
#define FIM_SYM_TYPE_INT	'i'
#define FIM_SYM_STRING_CONCAT	'.'
#define FIM_SYM_DOT_CHAR	'.'
#define FIM_SYM_SEMICOLON	';'
#define FIM_SYM_SEMICOLON_STRING	";"
#define FIM_SYM_ENDL	"\n"

/*
 * External programs used by fim.
 */
#define FIM_EPR_FIG2DEV		"fig2dev"
#define FIM_EPR_XCFTOPNM	"xcftopnm"
#define FIM_EPR_DIA		"dia"
#define FIM_EPR_INKSCAPE	"inkscape"
#define FIM_EPR_CONVERT		"convert"
#define FIM_EPR_ZCAT		"zcat"

/*
 * Some Fim error codes.
 */
#define FIM_ERR_NO_ERROR	0
#define FIM_ERR_GENERIC	-1
#define FIM_ERR_BUFFER_FULL	-1024 // FIXME: -2 seems in use

/*
 * Some Fim error messages.
 */
#define FIM_EMSG_NO_SCRIPTING	"sorry, no scripting available!\n"
#define FIM_EMSG_NO_READ_STDIN_IMAGE	"sorry, the reading of images from stdin was disabled at compile time\n"
#define FIM_EMSG_CACHING_STDIN	"problems caching standard input image!\n"
#define FIM_EMSG_OUT_OF_MEM	"out of memory\n"
#define FIM_EMSG_UNFINISHED	"sorry, feature incomplete!\n"

/*
 * Some environment variables used by Fim.
 */
#define FIM_ENV_DISPLAY "DISPLAY"
#define FIM_ENV_FRAMEBUFFER "FRAMEBUFFER"
#define FIM_ENV_FBGAMMA "FBGAMMA"
#define FIM_ENV_FBFONT "FBFONT"

/*
 * Fim Option (long) Switches
 * */
#define FIM_OSW_OUTPUT_DEVICE	"output-device"
#define FIM_OSW_BINARY	"binary"
#define FIM_OSW_EXECUTE_COMMANDS	"execute-commands"
#define FIM_OSW_EXECUTE_COMMANDS_EARLY	"execute-commands-early"
#define FIM_OSW_EXECUTE_SCRIPT	"execute-script"
#define FIM_OSW_FINAL_COMMANDS	"final-commands"
#define FIM_OSW_SCRIPT_FROM_STDIN	"script-from-stdin"
#define FIM_OSW_IMAGE_FROM_STDIN	"image-from-stdin"
#define FIM_OSW_DUMP_SCRIPTOUT "write-scriptout"

/*
 * Fim Constants
 * */
#define FIM_CNS_FIM	 "FIM - Fbi IMproved"
#define FIM_CNS_FIM_TXT	 "FIM.TXT"
#define FIM_CNS_BUGS_FILE	 "BUGS"
#define FIM_CNS_TERM_VAR	 "TERM"
#define FIM_CNS_HOME_VAR	 "HOME"
#define FIM_CNS_HIST_FILENAME	 ".fim_history"
#define FIM_CNS_SYS_RC_FILEPATH	 "/etc/fimrc"
#define FIM_CNS_DOC_PATH	 "/usr/local/share/doc/fim"	/* FIXME: shall depend on config.h */
#define FIM_CNS_USR_RC_FILEPATH	 ".fimrc"
#define FIM_CNS_USR_RC_COMPLETE_FILEPATH	 "~/.fimrc"
#define FIM_CNS_EXAMPLE_FILENAME	 "file.jpg"
#define FIM_CNS_SCALEFACTOR	 1.322f
#define FIM_CNS_SCALEFACTOR_ONE 1.0f
#define FIM_CNS_ANGLE_ONE 1.0f
#define FIM_CNS_ANGLE_ZERO 0.0f
#define FIM_CNS_GAMMA_DEFAULT 1.0
#define FIM_CNS_GAMMA_DEFAULT_STR FIM_XSTRINGIFY(FIM_CNS_GAMMA_DEFAULT)
#define FIM_CNS_SCALEFACTOR_MULTIPLIER 1.1f
#define FIM_CNS_SCALEFACTOR_DELTA 0.1f
#define FIM_CNS_SCALEFACTOR_ZERO 0.0f
//#define FIM_CNS_STEPS_DEFAULT	 	50
#define FIM_CNS_SCROLL_DEFAULT	 	"90%"
#define FIM_CNS_STEPS_DEFAULT_N	 	50
#define FIM_CNS_STEPS_DEFAULT	 	"20%"
#define FIM_CNS_STEPS_MIN	 1
#define FIM_CNS_WGROW_STEPS_DEFAULT	 1
#define FIM_CNS_WENLARGE_STEPS_DEFAULT	 10
#define FIM_CNS_SCALEDELTA	 0.01f
#define FIM_CNS_EMPTY_STRING	""
#define FIM_CNS_DEFAULT_IFNAME	FIM_CNS_EMPTY_STRING
#define FIM_CNS_SLASH_STRING	"/"
#define FIM_CNS_DIRSEP_STRING	FIM_CNS_SLASH_STRING
#define FIM_CNS_FP_ZERO		0.0
#define FIM_CNS_EMPTY_FP_VAL	FIM_CNS_FP_ZERO
#define FIM_CNS_ERR_QUIT	0
#define FIM_CNS_EMPTY_RESULT	FIM_CNS_EMPTY_STRING
#ifdef SVN_REVISION
#define FIM_CNS_FIM_APPTITLE FIM_CNS_FIM", v."PACKAGE_VERSION" (r."SVN_REVISION")"
#else
#define FIM_CNS_FIM_APPTITLE FIM_CNS_FIM", v."PACKAGE_VERSION""
#endif
#ifdef SVN_REVISION_NUMBER
#define FIM_REVISION_NUMBER SVN_REVISION_NUMBER
#else
#define FIM_REVISION_NUMBER -1
#endif
#define FIM_CNS_EX_KSY_STRING	"{keysym}"
#define FIM_CNS_EX_CMD_STRING	"{command}"
#define FIM_CNS_EX_FN_STRING	"{filename}"
#define FIM_CNS_EX_FNS_STRING	"{filename(s)}"
#define FIM_CNS_EX_KC_STRING	"{keycode}"
#define FIM_CNS_EX_ID_STRING	"{identifier}"
#define FIM_CNS_EX_EXP_STRING	"{expression}"
#define FIM_CNS_EX_PAT_STRING	"{pattern}"
#define FIM_CNS_EX_CMDS_STRING	"{commands}"
#define FIM_CNS_EX_EVT_STRING	"{event}"
#define FIM_CNS_EX_ARGS_STRING	"{args}"
#define FIM_CNS_EX_DSC_STRING	"{description}"
#define FIM_CNS_EX_PATH_STRING	"{path}"
#define FIM_CNS_EX_SYSC_STRING	"{syscmd}"
#define FIM_CNS_EX_FCT_STRING	"{factor}"
#define FIM_CNS_EX_RE_STRING	"{regexp}"
#define FIM_CNS_EX_RES_STRING	"{regexp(s)}"
#define FIM_CNS_EX_NUM_STRING	"{number}"
#define FIM_CNS_EX_SCALE_STRING	"{scale}"
#define FIM_CNS_SHELL	"/bin/sh"
#define FIM_CNS_PUSHDIR_RE	"\\.JPG$|\\.PNG$|\\.GIF$|\\.BMP$|\\.TIFF$|\\.JPEG$|\\.JFIF$|\\.PPM$"

#define FIM_MAX(x,y)        ((x)>(y)?(x):(y))
#define FIM_MIN(x,y)        ((x)<(y)?(x):(y))
#define FIM_MOD(X,C)        ((((X)%(C))+(C))%(C))

#define FIM_INTERNAL_LANGUAGE_SHORTCUT_SHORT_HELP \
".nf\n"\
":"FIM_CNS_EX_NUM_STRING"       jump to "FIM_CNS_EX_NUM_STRING"^th image in the list\n"\
":^	        jump to first image in the list\n"\
":$	        jump to last image in the list\n"\
":*"FIM_CNS_EX_FCT_STRING"      scale the image by "FIM_CNS_EX_FCT_STRING"\n"\
":"FIM_CNS_EX_SCALE_STRING"%       scale the image to the desired "FIM_CNS_EX_SCALE_STRING"\n"\
":+"FIM_CNS_EX_SCALE_STRING"%       scale the image up to the desired percentage "FIM_CNS_EX_SCALE_STRING" (relatively to the original)\n"\
":-"FIM_CNS_EX_SCALE_STRING"%       scale the image down to the desired percentage "FIM_CNS_EX_SCALE_STRING" (relatively to the original)\n"\
"\n"\
"/"FIM_CNS_EX_RE_STRING"		 entering the pattern "FIM_CNS_EX_RE_STRING" (with /) makes fim jump to the next image whose filename matches "FIM_CNS_EX_RE_STRING"\n"\
"/*.png$		 entering this pattern (with /) makes fim jump to the next image whose filename ends with 'png'\n"\
"/png		 a shortcut for /.*png.*\n"\
"\n"\
"!"FIM_CNS_EX_SYSC_STRING"		executes the "FIM_CNS_EX_SYSC_STRING" quoted string as a \""FIM_CNS_SHELL"\" shell command\n"\
""

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
	extern fim_stream cout;
	extern fim_stream cerr;
	std::ostream& operator<<(std::ostream &os,const string& s);
}

/*
 * Fim language variable identifiers, with their help strings.
 * TODO: shall specify more information about these variables
 * */
//#define FIM_VID_NEWLINE 			"_newline"	/* "" */
//#define FIM_VID_TAB 				"_tab"	/* "" */
#define FIM_VID_RANDOM 				"random"	/* "[internal,out] a pseudorandom number" */
#define FIM_VID_BINARY_DISPLAY 			"_display_as_binary"	/* "[internal,in] will force loading of the specified files as pixelmaps (no image decoding will be performed); if 1, using one bit per pixel;  if 24, using 24 bits per pixel; otherwise will load and decode the files as usual" */
#define FIM_VID_CACHE_STATUS 			"_cache_status"		/* "[internal,out] string with current information on cache status" */
#define FIM_VID_DISPLAY_CONSOLE 		"_display_console"	/* "[internal,in] if 1, will display the output console" */
#define FIM_VID_DEVICE_DRIVER 			"_device_string"	/* "[internal,out] the current display device string" */
#define FIM_VID_DISPLAY_STATUS			"_display_status"	/* "[internal,in] if 1, will display the status bar" */
#define FIM_VID_PUSH_PUSHES_DIRS		"_push_pushes_dirs"	/* "[internal,in] if 1, the push command will also accept and push directories (using pushdir)" */
#define FIM_VID_SANITY_CHECK			"_do_sanity_check"	/* "[internal,in,experimental] if 1, will execute a sanity check on startup" */
#define FIM_VID_LAST_SYSTEM_OUTPUT		"_last_system_output"	/* "[internal,out,experimental] the standard output of the last call to the system command" */
#define FIM_VID_LOAD_DEFAULT_ETC_FIMRC 		"_load_default_etc_fimrc"	/* "[internal,in] if 1 at startup, will load /etc/fimrc, or equivalent system startup file" */
#define FIM_VID_DEFAULT_ETC_FIMRC 		"_sys_rc_file"		/* "[internal,in] string with the global configuration file name" */
#define FIM_VID_NO_RC_FILE			"_no_rc_file"		/* "[internal,in] if 1, the ~/.fimrc file will not be loaded at startup" */
#define FIM_VID_NO_EXTERNAL_LOADERS		"_no_external_loader_programs"		/* "[internal,in] if 1, no external loading programs will be tried for piping in an unsupported type image file" */
#define FIM_VID_SCRIPTOUT_FILE			"_fim_scriptout_file"	/* "[internal,in] the name of the file to write to when recording sessions" */
#define FIM_VID_PUSHDIR_RE			"_pushdir_re"	/* "[internal,in] regular expression to match against when pushing files from a directory" */
#define FIM_VID_STATUS_LINE 			"_status_line"		/* "[internal,in] if 1, will display the status bar" */
#define FIM_VID_WANT_PREFETCH 			"_want_prefetch"	/* "[internal,in] if 1, will prefetch further files just after display of the first file" */
#define FIM_VID_WANT_SLEEPS 			"_want_sleep_seconds"	/* "[internal,in] number of seconds of sleep during slideshow mode" */
#define FIM_VID_AUTOTOP				"_autotop"		/* "[internal,in] if 1, will align to the top freshly loaded images" */
#define FIM_VID_SCALE_STYLE			"_scale_style"		/* "[internal,in] if non empty, this string will be fed to the scale command" */
#define FIM_VID_FILEINDEX			"_fileindex"		/* "[internal,out] the current image numeric index" */
#define FIM_VID_FILELISTLEN			"_filelistlen"		/* "[internal,out] the length of the current image list" */
#define FIM_VID_FILENAME			"_filename"		/* "[internal,out] the current file name string" */
#define FIM_VID_FIM_DEFAULT_CONFIG_FILE_CONTENTS "_fim_default_config_file_contents"/* "[internal,out] the contents of the default (hardcoded) configuration file (executed after the minimal hardcoded config)" */
#define FIM_VID_FIM_DEFAULT_GRAMMAR_FILE_CONTENTS "_fim_default_grammar_file_contents" /* "[internal,out] the contents of the default (hardcoded) grammar file" */
#define FIM_VID_FRESH				"fresh"			/* "[internal,in,out,experimental] 1 if the image was loaded, before all autocommands execution" */
#define FIM_VID_PAGE				"page"			/* "[internal,out,experimental] the current page" */
#define FIM_VID_PAGECOUNT			"pagecount"			/* "[internal,out,experimental] the page count for a given image" */
#define FIM_VID_OVERRIDE_DISPLAY		"_inhibit_display"	/* "[internal] if 1, will inhibit display" */
#define FIM_VID_MAX_ITERATED_COMMANDS		"_max_iterated_commands"	/* "[internal,experimental] the iteration limit for N in \"N[commandname]\" iterated command invocations" */
#define FIM_VID_MAGNIFY_FACTOR			"_magnify_factor"	/* "[internal,in] the image scale multiplier used when magnifying images size" */
#define FIM_VID_PWD				"_pwd"			/* "[internal,out] the current working directory; will be updated at startup and whenever the working directory changes" */
#define FIM_VID_REDUCE_FACTOR			"_reduce_factor"		/* "[internal,in] the image scale multiplier used when reducing images size" */
#define FIM_VID_SCALE_FACTOR_MULTIPLIER		"_scale_factor_multiplier"	/* "[internal,in] value used for scaling up/down the scaling factors" */
#define FIM_VID_SCALE_FACTOR_DELTA		"_scale_factor_delta"		/* "[internal,in] value used for incrementing/decrementing the scaling factors" */
#define FIM_VID_COMMENT 				"_comment"				/* "[internal,out] the image comment, extracted from the image file (if any)" */
#define FIM_VID_STEPS 				"_steps"				/* "[internal,in] the default steps, in pixels, when panning images" */
#define FIM_VID_VERSION				"_fim_version"	/* "[internal,out] fim version number; may be used for keeping compatibility of fim scripts across evolving versions."  */
#define FIM_VID_HSTEPS 				"_hsteps"				/* "[internal,in] the default steps, in pixels, when panning images horizontally (overrides steps)" */
#define FIM_VID_VSTEPS 				"_vsteps"				/* "[internal,in] the default steps, in pixels, when panning images vertically (overrides steps)" */
#define FIM_VID_CONSOLE_ROWS 			"_rows"			/* "[internal,in] if >0, will set the number of displayed text lines in the console" */
#define FIM_VID_CONSOLE_LINE_WIDTH 		"_lwidth"		/* "[internal,in] if>0, will force the output console text width" */
#define FIM_VID_CONSOLE_LINE_OFFSET 		"_console_offset"	/* "[internal,in,out] position of the text beginning in the output console, expressed in lines" */
#define FIM_VID_CONSOLE_BUFFER_LINES		"_console_lines"		/* "[internal,out] the number of buffered output console text lines" */
#define FIM_VID_CONSOLE_BUFFER_TOTAL		"_console_buffer_total"		/* "[internal,out] amount of memory allocated for the output console buffer" */
#define FIM_VID_CONSOLE_BUFFER_FREE		"_console_buffer_free"		/* "[internal,out] amount of unused memory in the output console buffer" */
#define FIM_VID_CONSOLE_BUFFER_USED		"_console_buffer_used"		/* "[internal,out] amount of used memory in the output console buffer" */
#define FIM_VID_VERBOSE_KEYS			"_verbose_keys"			/* "[internal,in] if 1, after each interactive mode key hit, the console will display the hit key raw keycode" */
#define FIM_VID_CMD_EXPANSION			"_command_expansion"			/* "[internal,in] if 1, will enable autocompletion (on execution) of alias and command strings" */
#define FIM_VID_VERBOSE_ERRORS			"_verbose_errors"			/* "[internal,in] if 1, will display on stdout internal errors, while parsing commands" */
#define FIM_VID_CONSOLE_KEY			"_console_key"		/* "[internal,in] the key binding (an integer variable) for spawning the command line; will have precedence over any other binding" */
#define FIM_VID_IGNORECASE			"_ignorecase"		/* "[internal,in] if 1, will allow for case insensitive regexp-based searches" */
#define FIM_VID_SAVE_FIM_HISTORY		"_save_fim_history"	/* "[internal,in] if 1 on exit, will save the ~/fim_history file on exit" */
#define FIM_VID_LOAD_FIM_HISTORY		"_load_fim_history"	/* "[internal,in] if 1 on startup, will load the ~/fim_history file on startup" */
#define FIM_VID_TERM				"_TERM"			/* "[internal,out] the environment TERM variable" */
#define FIM_VID_NO_DEFAULT_CONFIGURATION	"_no_default_configuration"	/* "[internal,in] if 0, a default, hardcoded configuration will be executed at startup, after the minimal hardcoded one. " */
#define FIM_VID_DISPLAY_STATUS_BAR		"_display_status_bar"		/* "[internal,in] if 1, will display the status bar" */
#define FIM_VID_DISPLAY_BUSY			"_display_busy"			/* "[internal,in] if 1, will display a message on the status bar when processing" */
#define FIM_VID_SCALE				"scale"				/* "[internal,in] the scale of the current image" */
#define FIM_VID_ASCALE				"ascale"			/* "[internal,in,out] the asymmetric scaling of the current image" */
#define FIM_VID_ANGLE				"angle"				/* "[internal,in,out] a floating point number specifying the rotation angle, in degrees" */
#define FIM_VID_ORIENTATION			"_orientation"		/* "[internal] Rotation is controlled by: 'i:_orientation', 'v:_orientation', 'g:_orientation' and applied with a per-image basis.  In particular, the values of the three variables are summed up and the sum is interpreted as the image orientation.  If the sum is 0, no rotation will apply; if it is 1, a single ( 90') rotation will apply; if it is 2, a double (180') rotation will apply; if it is 3, a triple (270') rotation will apply.  If the sum is not one of 0,1,2,3, the value of the sum modulo 4 is considered.  Therefore, \":i:_orientation=1\" and \":i:_orientation=5\" will do the same thing: rotate the image one time by 90'." */
#define FIM_VID_WIDTH				"width"			/* "[internal,out] the current image original width" */
#define FIM_VID_HEIGHT				"height"		/* "[internal,out] the current image original height" */
#define FIM_VID_SWIDTH				"swidth"		/* "[internal,out] the current image scaled width" */
#define FIM_VID_SHEIGHT				"sheight"		/* "[internal,out] the current image scaled height" */
#define FIM_VID_AUTOFLIP			"_autoflip"		/* "[internal,in] if 1, will flip images by default" */
#define FIM_VID_AUTONEGATE			"_autonegate"		/* "[internal,in] if 1, will negate images by default" */
#define FIM_VID_FLIPPED				"flipped"		/* "[internal,out] 1, if the image is flipped" */
#define FIM_VID_NEGATED				"negated"		/* "[internal,out] 1, if the image is negated" */
#define FIM_VID_FIM_BPP				"_fim_bpp"		/* "[internal,out] the bits per pixel count" */
#define FIM_VID_AUTOMIRROR			"_automirror"		/* "[internal,in] if 1, will mirror images by default" */
#define FIM_VID_MIRRORED			"mirrored"		/* "[internal,out] 1, if the image is mirrored " */
#define FIM_VID_WANT_AUTOCENTER			"_want_autocenter"	/* "[internal,in] if 1, the image will be displayed centered " */
#define FIM_VID_MAX_CACHED_IMAGES		"_max_cached_images"	/* "[internal,in] the maximum number of images allowed in the cache" */
#define FIM_VID_MAX_CACHED_MEMORY		"_max_cached_memory"	/* "[internal,in] the maximum amount of memory allowed for the cache" */
#define FIM_VID_CACHED_IMAGES			"_cached_images"	/* "[internal,out] the number of images currently cached" */
#define FIM_VID_SCREEN_WIDTH			"_screen_width"		/* "[internal,out] the screen width"  */
#define FIM_VID_SCREEN_HEIGHT			"_screen_height"		/* "[internal,out] the screen height" */
#define FIM_VID_DBG_AUTOCMD_TRACE_STACK		"_autocmd_trace_stack"	/* "[internal,in] dump to stdout autocommands stack trace during their execution (for debugging purposes)" */
#define FIM_VID_DBG_COMMANDS			"_debug_commands"	/* "[internal,in] print out each command before its execution (for debugging purposes)" */
#define FIM_VID_OPEN_OFFSET			"_open_offset"		/* "[internal,in,optional] offset (specified in bytes) used when opening a file " */
#define FIM_VID_SEEK_MAGIC			"_seek_magic"		/* "[internal,optional] will seek for a magic signature before opening a file (for now, use like this: fim -c '_seek_magic=MAGIC_STRING;push file_to_seek_in.ext' ) " */

/*
 * Fim Keyboard Descriptions
 * */
#define FIM_KBD_TAB			"Tab"
#define FIM_KBD_ENTER			"Enter"
#define FIM_KBD_BACKSPACE		"BackSpace"
#define FIM_KBD_BACKSPACE_		"Backspace"
#define FIM_KBD_DEL			"Del"
#define FIM_KBD_INS			"Ins"
#define FIM_KBD_HOME			"Home"
#define FIM_KBD_END			"End"
#define FIM_KBD_ANY			"Any"
#define FIM_KBD_ESC			"Esc"
#define FIM_KBD_LEFT			"Left"
#define FIM_KBD_RIGHT			"Right"
#define FIM_KBD_UP			"Up"
#define FIM_KBD_DOWN			"Down"
#define FIM_KBD_PAGEUP			"PageUp"
#define FIM_KBD_PAGEDOWN			"PageDown"
#define FIM_KBD_COLON			":"
#define FIM_KBD_MOUSE_LEFT			"MouseLeft"
#define FIM_KBD_MOUSE_RIGHT			"MouseRight"

/*
 * Fim Display Driver Names
 * */
#define FIM_DDN_INN_FB	 "fb"
#define FIM_DDN_VAR_FB	 "fb"
#define FIM_DDN_INN_SDL	 "sdl"
#define FIM_DDN_VAR_SDL	 "sdl"
#define FIM_DDN_INN_AA	 "aa"
#define FIM_DDN_VAR_AA	 "aa"
#define FIM_DDN_INN_CACA	 "caca"
#define FIM_DDN_VAR_CACA	 "caca"
#define FIM_DDN_INN_DUMB	 "dumb"
//#define FIM_DDN_VAR_DUMB	 "dummy"
#define FIM_DDN_VAR_DUMB	 "dumb"
#define FIM_DDN_VARS	 "[" FIM_DDN_INN_FB"|" FIM_DDN_INN_SDL "|" FIM_DDN_INN_AA "|" FIM_DDN_INN_DUMB "]" 
//#define FIM_DDN_VARS	 "[" FIM_DDN_INN_FB"|" FIM_DDN_INN_SDL "|" FIM_DDN_INN_AA "|" FIM_DDN_INN_CACA "|" FIM_DDN_INN_DUMB "]" 

/*
 * Fim Autocommands
 * FIXME: need autodocumentation for these.
 * */
//#define FIM_ACM_POSTSCREENRESIZE	"PostScreenResize"	/* "" */
//#define FIM_ACM_PRESCREENRESIZE		"PreScreenResize"	/* "" */
#define FIM_ACM_POSTSCALE	"PostScale"		/* "" */
#define FIM_ACM_PRESCALE	"PreScale"		/* "" */
#define FIM_ACM_PREPAN		"PrePan"		/* "" */
#define FIM_ACM_POSTPAN		"PostPan"		/* "" */
#define FIM_ACM_PREREDISPLAY	"PreRedisplay"		/* "" */
#define FIM_ACM_POSTREDISPLAY	"PostRedisplay"		/* "" */
#define FIM_ACM_PREDISPLAY	"PreDisplay"		/* "" */
#define FIM_ACM_POSTDISPLAY	"PostDisplay"		/* "" */
#define FIM_ACM_PREPREFETCH	"PrePrefetch"		/* "" */
#define FIM_ACM_POSTPREFETCH	"PostPrefetch"		/* "" */
#define FIM_ACM_POSTRELOAD	"PostReload"		/* "" */
#define FIM_ACM_PRERELOAD	"PreReload"		/* "" */
#define FIM_ACM_POSTLOAD	"PostLoad"		/* "" */
#define FIM_ACM_PRELOAD		"PreLoad"		/* "" */
#define FIM_ACM_POSTGOTO	"PostGoto"		/* "" */
#define FIM_ACM_PREGOTO		"PreGoto"		/* "" */
#define FIM_ACM_PRECONF		"PreConfigLoading"	/* "before loading any configuration file" */
#define FIM_ACM_POSTCONF	"PostConfigLoading"	/* "after  loading any configuration file" */
#define FIM_ACM_PREHFIMRC	"PreHardcodedConfigLoading"	/* "before loading hardcoded configuration" */
#define FIM_ACM_POSTHFIMRC	"PostHardcodedConfigLoading"	/* "after  loading hardcoded configuration" */
#define FIM_ACM_PREUFIMRC	"PreUserConfigLoading"		/* "before loading user configuration file" */
#define FIM_ACM_POSTUFIMRC	"PostUserConfigLoading"		/* "after  loading user configuration file" */
#define FIM_ACM_PREGFIMRC	"PreGlobalConfigLoading"	/* "before loading global configuration file" */
#define FIM_ACM_POSTGFIMRC	"PostGlobalConfigLoading"	/* "after  loading global configuration file" */
#define FIM_ACM_PREINTERACTIVECOMMAND	"PreInteractiveCommand"		/* "" */
#define FIM_ACM_POSTINTERACTIVECOMMAND	"PostInteractiveCommand"	/* "" */
#define FIM_ACM_PREEXECUTIONCYCLE	"PreExecutionCycle"		/* "" */
#define FIM_ACM_PREEXECUTIONCYCLEARGS	"PreExecutionCycleArgs"		/* "" */
#define FIM_ACM_POSTEXECUTIONCYCLE	"PostExecutionCycle"		/* "" */
#define FIM_ACM_PREWINDOW	"PreWindow"	/* "" */
#define FIM_ACM_POSTWINDOW	"PostWindow"	/* "" */
//#define FIM_ACM_PREROTATE	"PreRotate"	/* "" */
//#define FIM_ACM_POSTROTATE	"PostRotate"	/* "" */

/*
 * Fim Language Tokens
 * */
#define FIM_FLT_ALIAS			"alias"	/* not in vim */
#define FIM_FLT_ALIGN			"align"	/* not in vim */
#define FIM_FLT_AUTOCMD			"autocmd" /* in vim */
#define FIM_FLT_AUTOCMD_DEL		"autocmd_del"	/* not in vim */
#define FIM_FLT_BIND			"bind" /* not in vim */
#define FIM_FLT_BASENAME		"basename" /* not in vim */
#define FIM_FLT_CD			"cd" /* in vim */
#define FIM_FLT_CLEAR			"clear" /* not in vim */
#define FIM_FLT_COMMANDS		"commands" /* not in vim */
#define FIM_FLT_DISPLAY			"display" /* in vim, with another meaning */
#define FIM_FLT_DUMP_KEY_CODES		"dump_key_codes" /* not in vim */
#define FIM_FLT_ECHO			"echo" /* in vim */
#define FIM_FLT_ELSE			"else" /* in vim */
#define FIM_FLT_EXEC			"exec" /* not in vim */
#define FIM_FLT_EVAL			"eval" /* not in vim */
#define FIM_FLT_FILE			"file" /* in vim */
#define FIM_FLT_GETENV			"getenv" /* not in vim */
#define FIM_FLT_GOTO			"goto" /* in vim */
#define FIM_FLT_HELP			"help" /* not in vim */
#define FIM_FLT_IF			"if" /* in vim */
#define FIM_FLT_INFO			"info" /* not in vim */
#define FIM_FLT_LOAD			"load" /* not in vim */
#define FIM_FLT_LIST			"list" /* not in vim */
#define FIM_FLT_NEGATE			"negate" /* not in vim */
#define FIM_FLT_NO_IMAGE		"no_image" /* not in vim */
#define FIM_FLT_PAN			"pan" /* not in vim */
#define FIM_FLT_POPEN			"popen" /* not in vim */
#define FIM_FLT_PREAD			"pread" /* not in vim */
#define FIM_FLT_PREFETCH		"prefetch" /* in vim */
#define FIM_FLT_PWD			"pwd" /* in vim */
#define FIM_FLT_REDISPLAY		"redisplay" /* not in vim */
#define FIM_FLT_RELOAD			"reload" /* not in vim */
#define FIM_FLT_ROTATE			"rotate" /* not in vim */
#define FIM_FLT_SCALE			"scale" /* not in vim */
#define FIM_FLT_SCROLLDOWN		"scrolldown" /* not in vim */
#define FIM_FLT_SCROLLFORWARD		"scrollforward" /* not in vim */
#define FIM_FLT_SET			"set" /* in vim */
#define FIM_FLT_SET_INTERACTIVE_MODE	"set_interactive_mode" /* not in vim */
#define FIM_FLT_SET_CONSOLE_MODE	"set_commandline_mode" /* not in vim */
#define FIM_FLT_STATUS			"status" /* not in vim */
#define FIM_FLT_STDOUT			"stdout" /* not in vim */
#define FIM_FLT_QUIT			"quit" /* in vim */
#define FIM_FLT_RECORDING		"recording" /* not in vim */
#define FIM_FLT_SYSTEM			"system" /* not in vim */
#define FIM_FLT_SLEEP			"sleep" /* in vim */
#define FIM_FLT_USLEEP			"usleep" /* not in vim */
#define FIM_FLT_UNALIAS			"unalias" /* not in vim */
#define FIM_FLT_UNBIND			"unbind" /* not in vim */
#define FIM_FLT_VARIABLES		"variables" /* not in vim */
#define FIM_FLT_WHILE			"while" /* in vim */
#define FIM_FLT_WINDOW			"window" /* not in vim */

/* composite commands or hardcoded aliases */
#define FIM_FLA_NEXT_FILE		"next_file" /* not in vim */ // WAS: FIM_FLT_NEXT_PIC
#define FIM_FLA_PREV_FILE		"prev_file" /* not in vim */ // WAS: FIM_FLT_PREC_PIC
#define FIM_FLA_NEXT_PAGE		"next_page" /* not in vim */
#define FIM_FLA_PREV_PAGE		"prev_page" /* not in vim */
#define FIM_FLA_NEXT			"next" /* in vim */
#define FIM_FLA_PREV			"prev" /* in vim */
#define FIM_FLA_MAGNIFY			"magnify" /* not in vim */
#define FIM_FLA_REDUCE			"reduce" /* not in vim */
#define FIM_FLC_NEXT			"goto '+1'" /* in vim */
#define FIM_FLC_PREV			"goto '-1'" /* in vim */
#define FIM_FLC_MIRROR			"mirror" /* not in vim */
#define FIM_FLC_FLIP			"flip" /* not in vim */
#define FIM_FLC_PAN_UP			"pan 'up'" /* not in vim */
#define FIM_FLC_PAN_DOWN		"pan 'down'" /* not in vim */
#define FIM_FLC_PAN_LEFT		"pan 'left'" /* not in vim */
#define FIM_FLC_PAN_RIGHT		"pan 'right'" /* not in vim */
#define FIM_FLC_NEXT_FILE		"goto '+1f'" /* not in vim */ // WAS: FIM_FLT_NEXT_PIC
#define FIM_FLC_PREV_FILE		"goto '-1f'" /* not in vim */ // WAS: FIM_FLT_PREC_PIC
#define FIM_FLC_NEXT_PAGE		"goto '+1p'" /* not in vim */
#define FIM_FLC_PREV_PAGE		"goto '-1p'" /* not in vim */
#define FIM_FLC_MAGNIFY			"scale '+'" /* not in vim */
#define FIM_FLC_REDUCE			"scale '-'" /* not in vim */

/*
 * Help messages for Fim commands (partial).
 * One glorious day these macros will serve to build automatically documentation.
 * */
#define FIM_CMD_HELP_ALIGN FIM_FLT_ALIGN" bottom : align to the lower side the current image; " FIM_FLT_ALIGN" top : align to the upper side the current image; "
#define FIM_CMD_HELP_LIST	FIM_FLT_LIST" : display the files list; "FIM_FLT_LIST" random_shuffle: randomly shuffle the file list; "FIM_FLT_LIST" reverse: reverse the file list; "FIM_FLT_LIST" sort: sort the file list; "FIM_FLT_LIST" pop : pop the last file from the files list; " FIM_FLT_LIST" remove ["FIM_CNS_EX_FNS_STRING"] : remove the current file, or the "FIM_CNS_EX_FNS_STRING", if specified " "; push "FIM_CNS_EX_FNS_STRING" : push "FIM_CNS_EX_FNS_STRING" to the back of the files list; "FIM_FLT_LIST" filesnum : display the number of files in the files list; "FIM_FLT_LIST" mark : mark the current file for stdout printing at exit; "FIM_FLT_LIST" unmark : unmark the current file, preventing from stdout printing at exit; "FIM_FLT_LIST" pushdir {dirname} : will push all the files in {dirname}, when matching the regular expression in variable "FIM_VID_PUSHDIR_RE" or, if empty, from constant regular expression "FIM_CNS_PUSHDIR_RE" "
#define FIM_CMD_HELP_CD			FIM_FLT_CD" "FIM_CNS_EX_PATH_STRING": change the current directory to "FIM_CNS_EX_PATH_STRING". "FIM_FLT_CD" - will change to the previous current directory (before the last \":"FIM_FLT_CD" "FIM_CNS_EX_PATH_STRING"\" command)"
#define FIM_CMD_HELP_SET			FIM_FLT_SET": returns a list of variables which are set; "FIM_FLT_SET" "FIM_CNS_EX_ID_STRING": returns the value of variable "FIM_CNS_EX_ID_STRING"; "FIM_FLT_SET" "FIM_CNS_EX_ID_STRING" "FIM_CNS_EX_CMDS_STRING": sets variable "FIM_CNS_EX_ID_STRING" to value "FIM_CNS_EX_CMDS_STRING"; " 
#define FIM_CMD_HELP_PWD			FIM_FLT_PWD" : print the current directory name, and updates the "FIM_VID_PWD" variable"
#define FIM_CMD_HELP_EVAL			FIM_FLT_EVAL" "FIM_CNS_EX_ARGS_STRING" : evaluate "FIM_CNS_EX_ARGS_STRING" as commands, executing them"
#define FIM_CMD_HELP_SYSTEM		FIM_FLT_SYSTEM" "FIM_CNS_EX_SYSC_STRING": get the output of the shell command "FIM_CNS_EX_SYSC_STRING". (uses popen())"
#define FIM_CMD_HELP_WINDOW FIM_FLT_WINDOW" "FIM_CNS_EX_ARGS_STRING" : manipulates the window system windows; each value of "FIM_CNS_EX_ARGS_STRING" shall be one of ['split' | 'hsplit' | 'vsplit' | 'normalize' | 'enlarge' | 'venlarge' | 'henlarge' | 'up' | 'down' | 'left' | 'right' | 'close' | 'swap']"
#define FIM_CMD_HELP_GOTO 	FIM_FLT_GOTO" {['+'|'-']"FIM_CNS_EX_NUM_STRING"['%']['f'|'p']} | {/"FIM_CNS_EX_RE_STRING"/} | {'+//'}: jump to an image; if "FIM_CNS_EX_NUM_STRING" is given, and not surrounded by any specifier, will go to image at index "FIM_CNS_EX_NUM_STRING" ; if followed by '%', the effective index will be computed as a percentage to the current available images; if prepended by '-' or '+', the jump will be relative to the current index; the 'f' specifier asks for the jump to occur within the files; the 'p' specifier asks for the jump to occur in terms of pages, within the current file; if /"FIM_CNS_EX_RE_STRING"/ is given, will jump to the first image matching the given /"FIM_CNS_EX_RE_STRING"/ regular expression pattern; if given '+//', will jump to the first different image matching the last given regular expression pattern"
#define FIM_CMD_HELP_SCALE	FIM_FLT_SCALE" {['+'|'-']{value}['%']|'w'|'h'|'a'|'++'|'+-'|'+*'|'+/'} : scale the image according to a scale {value} (e.g.: 0.5,40%,'w','h','a'); if given 'w', will scale according to the screen width; if given 'h', scale to the screen height; if given 'a', to the minimum of 'w' and 'h'; if {value} is a number, will scale relatively to the original image width; if the number is followed by '%', the relative scale will be treated on a percent scale; " "if given '++'('+-'), will increment (decrement) the \""FIM_VID_MAGNIFY_FACTOR"\", \""FIM_VID_REDUCE_FACTOR"\" variables by \""FIM_VID_SCALE_FACTOR_DELTA"\"; " "if given '+*'('+/'), will multiply (divide) the \""FIM_VID_MAGNIFY_FACTOR"\", \""FIM_VID_REDUCE_FACTOR"\" variables by \""FIM_VID_SCALE_FACTOR_MULTIPLIER"\"; "
#define FIM_CMD_HELP_HELP	FIM_FLT_HELP" ["FIM_CNS_EX_ID_STRING"] : provide online help, if "FIM_CNS_EX_ID_STRING" is some variable, alias, or command identifier"


#define FIM_CNS_SLIDESHOW_CMD "while("FIM_VID_FILEINDEX"<"FIM_VID_FILELISTLEN"){sleep "FIM_VID_WANT_SLEEPS"; next;}"

/*
 * Some Fim compilation defaults
 */
#define FIM_WANT_SDL_PROOF_OF_CONCEPT_MOUSE_SUPPORT 1 /* experimental mouse support in sdl mode */
#define FIM_WANT_SCREEN_KEY_REMAPPING_PATCH 1
#define FIM_WANT_OVERLY_VERBOSE_DUMB_CONSOLE 0
#define FIM_WANT_MILDLY_VERBOSE_DUMB_CONSOLE 1
#define FIM_WANT_SINGLE_SYSTEM_INVOCATION 1
#define FIM_WANT_SDL_OPTIONS_STRING 1
#define FIM_WANT_OUTPUT_DEVICE_STRING_CASE_INSENSITIVE 1
#define FIM_WANT_HISTORY 1
#define FIM_STREAM_BUFSIZE	4096
#define FIM_MAXLINE_BUFSIZE	1024
#define FIM_STRING_BUFSIZE	4096
#define FIM_PIPE_BUFSIZE	1024
#define FIM_CONSOLE_BLOCKSIZE	1024
#define FIM_CONSOLE_DEF_WIDTH	128
#define FIM_BITRENDERING_DEF_WIDTH	1024
#define FIM_RENDERING_DPI	72
#define FIM_RENDERING_MAX_ROWS	1024
#define FIM_RENDERING_MAX_COLS	1024
#define FIM_CONSOLE_DEF_ROWS	24
#define FIM_VERBOSE_KEYS_BUFSIZE	64
#define FIM_FILE_BUF_SIZE 	(1024*256)
#define FIM_ATOX_BUFSIZE 	(32)
#define FIM_STATUSLINE_BUF_SIZE 	(128)
#define FIM_FBI_PPM_LINEBUFSIZE 	(1024)
#define FIM_LINUX_CONSOLEFONTS_DIR "/usr/share/consolefonts"
#define FIM_LINUX_STDIN_FILE "/dev/stdin"
#define FIM_LINUX_STDOUT_FILE "/dev/stdout"
#define FIM_LINUX_RAND_FILE "/dev/urandom"
#define FIM_FBDEV_FILE_MAX_CHARS 16
#define FIM_DEFAULT_FB_FILE "/dev/fb0"
#define FIM_DEFAULT_AS_BINARY_BPP 24

/*
 * Various  Fim messages.
 */
#define FIM_MSG_CONSOLE_FIRST_LINE_BANNER "=== This is the fim output console. You can scroll it up and down. ===\n"
#define FIM_MSG_CONSOLE_LONG_LINE "\n============================================================\n"
#define FIM_MSG_WAIT_PIPING "please wait while piping through"

/*
 * Some Fim internals flags.
 */
#define FIM_FLAG_MIRROR 1
#define FIM_FLAG_FLIP 2
#define FIM_FLAG_RGB2GRAY 4
#define FIM_FLAG_RGB2GRAYGRAYGRAY 8

/* we wait for variadic macros support in standard C++ */
#define FIM_FPRINTF fprintf

/* symbolic wrappers for memory handling calls */
#define fim_calloc(x,y) calloc((x),(y))
#define fim_malloc(x) malloc(x)
#define fim_free(x) free(x)


#define FIM_XSTRINGIFY(X) FIM_STRINGIFY(X)
#define FIM_STRINGIFY(X) #X

#endif
