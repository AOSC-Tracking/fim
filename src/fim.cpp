/* $LastChangedDate$ */
/*
 fim.cpp : Fim main program and accessory functions

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

#include "fim.h"
#include <signal.h>
#include <getopt.h>
#ifdef FIM_READLINE_H
#include "readline.h"	/* readline stuff */
#endif
/*
 * We use the STL (Standard Template Library)
 */
using std :: endl;
using std :: ifstream;
using std :: ofstream;
using std :: map;
using std :: multimap;
using std :: pair;
using std :: vector;


/*
 * Global variables.
 * */
	fim::string g_fim_output_device;
	FlexLexer *lexer;

/*
 * (nearly) all Fim stuff is in the fim namespace.
 * */
namespace fim
{
	/*
	 * Globals : should be encapsulated.
	 * */
	fim::CommandConsole cc;
	char *default_fbdev=NULL,*default_fbmode=NULL;
	int default_vt=-1;
	float default_fbgamma=-1.0;
	fim_stream cout;
	fim_stream cerr(1);
}

struct fim_options_t{
  const char *name;
  int has_arg;
  int *flag;
  int val;
  const char *desc;/* this is fim specific */
  const char *optdesc;/* this is fim specific */
  const char *mandesc;/* this is fim specific */
};

/*
 * Yet unfinished. 
 * This structure keeps hold of Fim's options flags.
 */
struct fim_options_t fim_options[] = {
    {"autozoom",   no_argument,       NULL, 'a',"scale according to a best fit",NULL,
"Enable autozoom.  fim will automagically pick a reasonable zoom factor when loading a new image. (as in fbi)"
    },
    {FIM_OSW_BINARY,     optional_argument,       NULL, 'b',"view any file as either a 1 or 24 bpp bitmap","[=24|1]",
"Display (any filetype) binary files contents as they were raw 24 or 1 bits per pixel pixelmaps.\n" 
"Will pad with zeros.\n"
"Regard this as a toy..\n"
    },
    {FIM_OSW_EXECUTE_COMMANDS, required_argument,       NULL, 'c',"execute {commands} after initialization","{commands}",
"The \\fBcommands\\fP string will be executed before entering the interactive loop.\n"
"Please note that if your commands are more complicated than a simple 'next' or 'pornview'\n"
"command, they must be quoted and escaped in a manner suitable for your shell!\n"
"\n"
"For example,\n"
"-c '*2;2pan_up;display;while(1){bottom_align;sleep \"1\" ; top_align}'\n"
"\n"
"(with the single quotes) will tell fim to first double the displayed image \n"
"size, then pan two times up, then display the image ; and then \n"
"do an endless loop consisting of bottom and top aligning, alternated.\n"
    },
    {"device",     required_argument, NULL, 'd',"specify a {framebuffer device}","{framebuffer device}",
"framebuffer device to use.  Default is the one your vc is mapped to (as in fbi)."
    },
    {"dump-reference-help",      optional_argument /*no_argument*/,       NULL, 0xd15cbab3,"dump reference info","[=man]",
"Will dump to stdout the language reference help."
    },
    {"dump-default-fimrc",      no_argument,       NULL, 'D',"dump on standard output the default configuration",NULL,
"The default configuration (the one hardcoded in the fim executable) is dumped on standard output and fim exits."
    },
    {FIM_OSW_EXECUTE_SCRIPT,   required_argument,       NULL, 'E',"execute {scriptfile} after initialization","{scriptfile}",
"The \\fBscriptfile\\fP will be executed right after the default initialization file is executed."
    },
    {"etc-fimrc",       required_argument, NULL, 'f',"etc-fimrc read (experimental)","{fimrc}",
"Specify an alternative system wide initialization file (default: "FIM_CNS_SYS_RC_FILEPATH"), which will be executed prior to any other configuration file.\n"
    },
    {FIM_OSW_FINAL_COMMANDS,   required_argument,       NULL, 'F',"execute {commands} just before exit","{commands}",
"The \\fBcommands\\fP string will be executed after exiting the interactive loop of the program (right before terminating the program)."
    },
    {"help",       optional_argument,       NULL, 'h',"print short (or descriptive, or long, or complete man) program invocation help","[=s|d|l|m]",
"display help and terminate the program."
    },
#ifdef FIM_READ_STDIN_IMAGE
    {FIM_OSW_IMAGE_FROM_STDIN,      no_argument,       NULL, 'i',"read an image file from standard input",NULL,
"Will read one single image from the standard input (yes: the image, not the filename).  May not work with all supported file formats."
    },
#endif
    {"mode",       required_argument, NULL, 'm',"specify a video mode","{vmode}",
"name of the video mode to use video mode (must be listed in /etc/fb.modes).  Default is not to change the video mode.  In the past, the XF86 config file (/etc/X11/XF86Config) used to contain Modeline information, which could be fed to the modeline2fb perl script (distributed with fbset).  On many modern xorg based systems, there is no direct way to obtain a fb.modes file from the xorg.conf file.  So instead one could obtain useful fb.modes info by using the (fbmodes (no man page AFAIK)) tool, written by bisqwit.  An unsupported mode should make fim exit with failure.  But it is possible the kernel could trick fim and set a supported mode automatically, thus ignoring the user set mode."
    },
    {"no-rc-file",      no_argument,       NULL, 'N',"do not read the personal initialization file at startup",NULL,
"No personal initialization file will be read (default is "FIM_CNS_USR_RC_COMPLETE_FILEPATH") at startup."
    },
    {"no-etc-rc-file",      no_argument,       NULL, 0x4E4E,"do not read the system wide initialization file at startup",NULL,
"No system wide initialization file will be read (default is "FIM_CNS_SYS_RC_FILEPATH") at startup."
    },
    {FIM_OSW_SCRIPT_FROM_STDIN,      no_argument,       NULL, 'p',"read commands from standard input",NULL,
"Will read commands from stdin prior to entering in interactive mode."
    },
    {FIM_OSW_OUTPUT_DEVICE,      required_argument,       NULL, 'o',"specify using a specific output driver (if supported)",FIM_DDN_VARS,
"Will use the specified \\fBdevice\\fP as fim video output device, overriding automatic checks."
"The available devices depend on the original configuration/compilation options, so you should\n"
"get the list of available output devices issuing \\fBfim --version\\fP.\n"
"It will probably be a subset  of {\\fBsdl\\fP, \\fBfb\\fP, \\fBaa\\fP, \\fBcaca\\fP, \\fBdumb\\fP}.\n"
#if FIM_WANT_SDL_OPTIONS_STRING 
"The \\fBsdl\\fP option may be specified as  \\fBsdl"FIM_SYM_DEVOPTS_SEP_STR"{['w']['m']width:height}\\fP , where \\fBwidth\\fP is and \\fBheight\\fP are integer numbers specifying the desired resolution, and the \\fB'w'\\fP character requests windowed mode, and the \\fB'm'\\fP character requests mouse pointer display (experimental features).\n"
#endif
#if FIM_WANT_OUTPUT_DEVICE_STRING_CASE_INSENSITIVE
"You can use upper and lower case characters indifferently in the specification string (comparisons are case insensitive).\n"
#endif
    },
    {"offset",      required_argument,       NULL,  0xFFD8FFE0,"will open the first image file at the specified offset","{bytes-offset}",
"Will use the specified \\fBoffset\\fP (in bytes) for opening the specified files (useful for viewing images on damaged file systems; however, since the internal variables representation is sizeof(int) bytes based, you have a limited offset range: using already chopped image files may be a workaround to this limitation)."
    },/* NEW */
    {"text-reading",      no_argument,       NULL, 'P',"proceed scrolling as reading through a text document",NULL,
"Enable textreading mode.  This has the effect that fim will display images scaled to the width of the screen, and aligned to the top.  Useful if the images you are watching text pages, all you have to do to get the next piece of text is to press space (in the default key configuration, of course)."
    },
    {"scroll",     required_argument, NULL, 's',"set scroll value (in pixels)","{value}",
"set scroll steps in pixels (default is 50)."
    },
    {"slideshow",     required_argument, NULL, 0x7373,"interruptible slideshow mode",FIM_CNS_EX_NUM_STRING,
"interruptible slideshow mode; will wait for "FIM_CNS_EX_NUM_STRING" of seconds (assigned to the "FIM_VID_WANT_SLEEPS" variable after each loading; implemented by executing "FIM_CNS_SLIDESHOW_CMD" as a first command."
    },
    {"sanity-check",      no_argument,       NULL, 'S',"perform a sanity check",NULL,
"a quick sanity check before starting the interactive fim execution, but after the initialization."
    },	/* NEW */
    {"no-framebuffer",      no_argument,       NULL, 't',"display images in text mode (as -o aa)",NULL,
"Fim will not use the framebuffer but the aalib (ascii art) driver instead (if you are curious, see (info aalib)).\n"
"If aalib was not enabled at tompile time, fim will work without displaying images at all."
    },
    {"vt",         required_argument, NULL, 'T',"specify a virtual terminal for the framebufer","{terminal}",
"The \\fBterminal\\fP will be used as virtual terminal device file (as in fbi).\n"
"See (chvt (1)), (openvt (1)) for more info about this.\n"
"Use (con2fb (1)) to map a terminal to a framebuffer device.\n"
    },
    {"random",     no_argument,       NULL, 'u',"randomize images order",NULL,
"randomly shuffle the files list before browsing."
    },
    {"verbose",    no_argument,       NULL, 'v',"verbose mode",NULL,
"be verbose: show status bar."
    },
    {"version",    no_argument,       NULL, 'V',"print program version",NULL,
"display version and compile flags, and then terminate."
    },
    {"autowidth",   no_argument,       NULL, 'w',"scale according to width",NULL,
"Will adapt the image size to the screen size."
    },
    {FIM_OSW_DUMP_SCRIPTOUT,      required_argument,       NULL, 'W',"will record any executed command to the a {scriptfile}","{scriptfile}",
"All the characters that you type are recorded in the file {scriptout}, until you exit Fim.  This is  useful  if  you want to create a script file to be used with \"fim -c\" or \":exec\" (analogous to Vim's -s and \":source!\").  If the {scriptout} file exists, it will be not touched (as in Vim's -w). "
    },
#ifdef FIM_READ_STDIN
    {"read-from-stdin",      no_argument,       NULL, '-',"read an image list from standard input",NULL,
"Reads file list from stdin.\n"
"\n"
"Note that these the three standard input reading functionalities (-i,-p and -) conflict : if two or more of them occur in fim invocation, fim will exit with an error and warn about the ambiguity.\n"
"\n"
"See the section\n"
".B EXAMPLES\n"
"below to read some useful (and unique) ways of employing fim.\n"
    },
#endif
    {"autotop",   no_argument,       NULL, 'A',"align images to the top (UNFINISHED)",NULL,
	    NULL
    },
//    {"gamma",      required_argument, NULL, 'g',"set gamma (UNFINISHED)","{gamma}",
//" gamma correction.  Can also be put into the FBGAMMA environment variable.  Default is 1.0.  Requires Pseudocolor or Directcolor visual, doesn't work for Truecolor."
//    },
    {"quiet",      no_argument,       NULL, 'q',"quiet mode (UNFINISHED)",NULL,
	    NULL
    },
    {"resolution", required_argument, NULL, 'r',"set resolution (UNFINISHED)","{resolution}",
	    NULL
    },
/*    {"timeout",    required_argument, NULL, 't',"",NULL},*/  /* timeout value */	/* fbi's */
/*    {"once",       no_argument,       NULL, '1',"",NULL},*/  /* loop only once */
/*    {"font",       required_argument, NULL, 'f',"",NULL},*/  /* font */
/*    {"edit",       no_argument,       NULL, 'e',"",NULL},*/  /* enable editing */	/* fbi's */
/*    {"list",       required_argument, NULL, 'l',"",NULL},*/
//    {"backup",     no_argument,       NULL, 'b',"",NULL},	/* fbi's */
//    {"debug",      no_argument,       NULL, 'D',"",NULL},
//    {"preserve",   no_argument,       NULL, 'p',"",NULL},	/* fbi's */

    /* long-only options */
//    {"autoup",     no_argument,       &autoup,   1 },
//    {"autodown",   no_argument,       &autodown, 1 },
//    {"comments",   no_argument,       &comments, 1 },
    {0,0,0,0,0,0}
};

#if 0
// leftovers from the old man file; shall adapt these using .\"
 .TP
 .B -f font
 Set font.  This can be either a pcf console font file or a X11 font
 spec.  Using X11 fonts requires a font server (The one specified in
 the environment variable FONTSERVER or on localhost).  The FBFONT
 environment variable is used as default.  If unset, fim will
 fallback to 10x20 (X11) / lat1u-16.psf (console).
 .TP
 .B --autoup
 Like autozoom, but scale up only.
 .TP
 .B --autodown
 Like autozoom, but scale down only.
 .TP
 .B -u
 Randomize the order of the filenames.
 .TP
 .B -e
 Enable editing commands.
 .TP
 .B -b
 create backup files (when editing images).
 .TP
 .B -p
 preserve timestamps (when editing images).
 .TP
 .B --comments
 Display comment tags (if present) instead of the filename.  Probaby
 only useful if you added reasonable comments yourself (using wrjpgcom
 for example), otherwise you likely just find texts pointing to the
 software which created the image.
 P               pause the slideshow (if started with -t, toggle)
 {number}g    jump to image {number}
 .SH EDIT IMAGE
 fim also provides some very basic image editing facilities.  You have
 to start fim with the -e switch to use them.
 .P
 .nf
 Shift+D         delete image
 R               rotate 90° clockwise
 L               rotate 90° counter-clock wise
 .fi
 .P
 The delete function actually wants a capital letter 'D', thus you have
 to type Shift+D.  This is done to avoid deleting images by mistake
 because there are no safety bells:  If you ask fim to delete the image,
 it will be deleted without questions asked.
 .P
 The rotate function actually works for JPEG images only because it
 calls the jpegtran command to perform a lossless rotation if the image.
 It is especially useful if you review the images of your digital
 camera.
#endif

const int fim_options_count=sizeof(fim_options)/sizeof(fim_options_t);
struct option options[fim_options_count];


class FimInstance
{
	static void version();

string fim_dump_man_page_snippets()
{
	string ms;
	const char*helparg="m";
	const char*slob;
	const char*sloe;
	const char*slol;
	const char*slom;
	ms+=
".TP\n"
".B --\n"
"The arguments before\n"
".B --\n"
"beginning with \n"
".B -\n"
"will be treated as command line switches.\n"
"All arguments after\n"
".B --\n"
"will be treated as filenames regardlessly.\n"
".\n"
;
	if(*helparg=='m')
	{
		slol=".TP\n.B ";
		slob=" --";
		sloe="\n";
		slom="\n";
	}
	else
	{
		slol="\t-";
		slob="\t\t--";
		slom="";
		sloe="\n";
	}
	for(size_t i=0;i<fim_options_count-1;++i)
	{	
		if(isascii(fim_options[i].val))
		{
	   		if((fim_options[i].val)!='-')
			{
				ms+=slol,ms+="-",ms+=string((char)(fim_options[i].val));
				if(fim_options[i].has_arg==required_argument)
				{
					if(fim_options[i].optdesc) ms+=" ",ms+=fim_options[i].optdesc;
				}
				ms+=",";
			}
	 	  	else ms+=slol,ms+=" -, ";
		}
		else ms+=".TP\n.B \t";
		ms+=slob;
		ms+=fim_options[i].name ;
		switch(fim_options[i].has_arg){
		case no_argument:
		break;
		case required_argument:
		//std::cout << " <arg>";
		if(fim_options[i].optdesc) ms+=" ",ms+=fim_options[i].optdesc; else ms+=" <arg>";
		break;
		case optional_argument:
		if(fim_options[i].optdesc) ms+=fim_options[i].optdesc; else ms+="[=arg]";
		break;
		default:
		;
		};
		ms+=slom;
		if(helparg&&*helparg=='d')ms+="\t\t ",ms+=fim_options[i].desc;
		if(helparg&&*helparg=='m')
		{
			if(fim_options[i].mandesc)
				ms+=fim_options[i].mandesc;
			else
			{
				ms+="\t\t ";
				if(fim_options[i].desc)
					ms+=fim_options[i].desc;
			}
		}
		//if(helparg||*helparg!='m') ms+=FIM_SYM_ENDL;
		ms+=sloe;
		//if(helparg&&*helparg=='l') std::cout << "TODO: print extended help here\n";
	}
	ms+="\n";
	return ms;
}

int fim_dump_man_page()
{
	string mp=
			string(".\\\"\n")+
			string(".\\\" $Id""$\n")+
			string(".\\\"\n")+
			string(".TH fim 1 \"(c) 2007-2011 "FIM_AUTHOR_NAME"\"\n")+
			string(".SH NAME\n")+
			string("fim - \\fBf\\fPbi (linux \\fBf\\fPrame\\fBb\\fPuffer \\fBi\\fPmageviewer) \\fBim\\fPproved\n")+
			string(".SH SYNOPSIS\n")+
			string(".B fim [{options}] [--] {imagefile} [{imagefiles}]\n.fi\n")+
			string(".B ... | fim [{options}] [--] [{imagefiles}] -\n.fi\n")+
			string(".B fim [{options}] [--] [{files}] - < {file_name_list_text_file}\n.fi\n")+
			string(".B fim --"FIM_OSW_IMAGE_FROM_STDIN" [{options}] < {imagefile}\n.fi\n")+
			string(".B fim --"FIM_OSW_SCRIPT_FROM_STDIN" [{options}] < {scriptfile}\n.fi\n")+
			string("\n")+
			string(".SH DESCRIPTION\n")+
			string(".B\nfim\nis a `swiss army knife` for displaying image files.\n")+
			string("It is capable of displaying image files using a variety of output video modes, while attempting at offering a uniform look and feel; it features an internal command language specialized to the image viewing purposes; it is capable of interacting with standard input and output in a number of ways; the internal command language is accessible via a command line capable of autocompletion and history; it features command recording, supports initialization files, key bindings customization, internal variables and command aliases, vim-like autocommands, and much more.\n\n")+
			string("As a default,\n.B\nfim\ndisplays the specified file(s) on the linux console using the framebuffer device.  jpeg, ppm, gif, tiff, xwd, bmp and png are supported directly.\nFor 'xcf' (Gimp's) images, fim will try to use '"FIM_EPR_XCFTOPNM"'.\nFor '.fig' vectorial images, fim will try to use '"FIM_EPR_FIG2DEV"'.\nFor '.dia' vectorial images, fim will try to use '"FIM_EPR_DIA"'.\nFor '.svg' vectorial images, fim will try to use '"FIM_EPR_INKSCAPE"'.\nFor other formats fim will try to use ImageMagick's '"FIM_EPR_CONVERT"' executable.\n")+
			string("\n")+
			string("\n""If configured at build time, fim will be capable of using SDL or aalib output.\n\n")+
			string("Please note that a user guide of \n.B fim\nis in the "FIM_CNS_FIM_TXT" file distributed in the source package.  This man page only describes the\n.B fim\ncommand line switches.\n\n")+
			string("See man fimrc for a full specification of the \n.B fim\nlanguage, commands, and variables.\n")+
			string("\n.SH OPTIONS\n");
			mp+=fim_dump_man_page_snippets();
			mp+=string(".SH COMMON KEYS AND COMMANDS\n"
".nf\n"
"cursor keys     scroll large images\n"
"h,j,k,l		scroll large images left,down,up,right\n"
"+, -            zoom in/out\n"
"ESC, q          quit\n"
"Tab             toggle output console visualization\n"
"PgUp,p            previous image\n"
"PgDn,n            next image\n"
"Space  	        next image if on bottom, scroll down instead\n"
"Return          next image, write the filename of the current image to stdout on exit from the program.\n"
"m			mirror\n"
"f			flip\n"
"r			rotate by 90  degrees\n"
"d,x,D,X		diagonal scroll\n"
"C-w			scale to the screen width\n"
"H			scale to the screen heigth\n"
"m			mark the current file for printing its name when terminating fim\n"
"\n"
FIM_INTERNAL_LANGUAGE_SHORTCUT_SHORT_HELP
"\n"
"C-n		 after entering in search mode (/) and submitting a pattern, C-n (pressing the Control and the n key together) will jump to the next matching filename\n"
"C-c		 terminate instantaneously fim\n"
"T		 split horizontally the current window\n"
"V		 split vertically the current window\n"
"C		 close  the currently focused window\n"
"H		 change the currently focused window with the one on the left\n"
"J		 change the currently focused window with the lower\n"
"K		 change the currently focused window with the upper\n"
"L		 change the currently focused window with the one on the right\n"
"U		 swap the currently focused window with the split sibling one (it is not my intention to be obscure, but precise  : try V, m,  U and see by yourself :) )\n"
"d		move the image diagonally north-west\n"
"D		move the image diagonally south-east\n"
"x		move the image diagonally north-east\n"
"X		move the image diagonally south-west\n"
"m		mirror\n"
"f		flip\n"
"r		rotate\n"
"\n"
"You can visualize all of the default bindings invoking fim --dump-default-fimrc | grep bind .\n"
"You can visualize all of the default aliases invoking fim  --dump-default-fimrc | grep alias .\n"
"\n"
".fi\n"
".P\n"
"The Return vs. Space key thing can be used to create a file list while\n"
"reviewing the images and use the list for batch processing later on.\n"
"\n"
"All of the key bindings are reconfigurable; please see the default \n"
".B fimrc\n"
"file for examples on this, or read the complete manual: the FIM.TXT file\n"
"distributed with fim.\n"
					)+
			string(
".SH AFFECTING ENVIRONMENT VARIABLES\n"
".nf\n"
//""FIM_ENV_FBFONT"		(just like in fbi) a consolefont or a X11 (X Font Server - xfs) font file.\n"
""FIM_ENV_FBFONT"		(just like in fbi) a Linux consolefont font file.\n"
"If not specified, the following files will be probed and the first existing will be selected:\n\n");
mp+=get_default_font_list();
mp+="\n";
mp+=string(
//"			For instance,  /usr/share/consolefonts/LatArCyrHeb-08.psf.gz is a Linux console file.\n"
//"			Consult 'man setfont' for your current font paths.\n"
//"			NOTE : Currently xfs is disabled.\n"
""FIM_ENV_FBGAMMA"		(just like in fbi) gamma correction (applies to dithered 8 bit mode only). Default is "FIM_CNS_GAMMA_DEFAULT_STR".\n"
""FIM_ENV_FRAMEBUFFER"	(just like in fbi) user set framebuffer device file (applies only to the "FIM_DDN_INN_FB" mode).\n"
"If unset, fim will probe for "FIM_DEFAULT_FB_FILE".\n"
""FIM_CNS_TERM_VAR"		(only in fim) will influence the output device selection algorithm, especially if $"FIM_CNS_TERM_VAR"==\"screen\".\n"
#ifdef FIM_WITH_LIBSDL
""FIM_ENV_DISPLAY"	If this variable is set, then the "FIM_DDN_INN_SDL" driver will be tried by default.\n"
#endif
".SH COMMON PROBLEMS\n"
".B fim\n"
"needs read-write access to the framebuffer devices (/dev/fbN or /dev/fb/N), i.e you (our\n"
"your admin) have to make sure fim can open the devices in rw mode.\n"
"The IMHO most elegant way is to use pam_console (see\n"
"/etc/security/console.perms) to chown the devices to the user logged\n"
"in on the console.  Another way is to create some group, chown the\n"
"special files to that group and put the users which are allowed to use\n"
"the framebuffer device into the group.  You can also make the special\n"
"files world writable, but be aware of the security implications this\n"
"has.  On a private box it might be fine to handle it this way\n"
"through.\n"
"\n"
"If using udev, you can edit :\n"
"/etc/udev/permissions.d/50-udev.permissions\n"
"and set these lines like here :\n"
" # fb devices\n"
" fb:root:root:0600\n"
" fb[0-9]*:root:root:0600\n"
" fb/*:root:root:0600\n"
".P\n"
"\n"
".B fim\n"
"also needs access to the linux console (i.e. /dev/ttyN) for sane\n"
"console switch handling.  That is obviously no problem for console\n"
"logins, but any kind of a pseudo tty (xterm, ssh, screen, ...) will\n"
".B not\n"
"work.\n"
".SH EXAMPLES\n"
".B find /mnt/media/ -name *.jpg | fim - .\n"
".fi \n"
"# Will make fim read the file list from standard input.\n"
".P\n"
".P\n"
"\n"
".B\n"
"find /mnt/media/ -name *.jpg | shuf | fim -\n"
".fi\n"
"# will make fim read the file list from standard input, randomly shuffled.\n"
".P\n"
".P\n"
"\n"
".B\n"
"cat script.fim | fim -p images/*\n"
".fi\n"
"# Will make fim read the script file\n"
".B script.fim\n"
"from standard input prior to displaying files in the directory\n"
".B images\n"
".P\n"
".P\n"
"\n"
".B \n"
"scanimage ... | tee scan.ppm | fim -i\n"
".fi\n"
"# Will make fim read the image scanned from a flatbed scanner as soon as it is read \n"
".P\n"
".P\n"
"\n"
".B fim * > selection.txt\n"
".fi\n"
"# Will output the file names marked interactively with the '"FIM_FLT_MARK"' command in fim to a file.\n"
".P\n"
".P\n"
"\n"
".B fim * | fim -\n"
".fi\n"
"# will output the file names marked with 'm' in fim to a second instance of fim, in which these could be marked again.\n"
".P\n"
".P\n"
"\n"
".B fim\n-c 'pread \"vgrabbj -d /dev/video0 -o png\";reload'\n"
".fi\n"
"# will display an image grabbed from a webcam.\n"
".P\n"
".P\n"
"\n"
".B fim\n-o aa -c 'pread \"vgrabbj -d /dev/video0 -o png\";reload;system \"fbgrab\" \"asciime.png\"'\n"
".fi\n"
"# if running in framebuffer mode, will save a png screenshot with an ascii rendering of an image grabbed from a webcam.\n"
".P\n"
".P\n"
"\n"
".B fim\n"
"-c 'while(1){pread \"vgrabbj -d /dev/video0 -o png\";reload;sleep 1;};'\n"
".fi\n"
"# will display a sequence of images grabbed from a webcam; circa 1 per second.\n"
".P\n"
".P\n"
"\n"
".SH NOTES\n"
"This manual page is neither accurate nor complete. In particular, issues related to driver selection shall be described more accurately. Also the accurate sequence of autocommands execution, variables application is critical to understanding fim, and should be documented.\n"
".SH BUGS\n"
".B fim\n"
"has bugs. Please read the \n"
".B BUGS\n"
"file shipped in the documentation directory to discover the known ones.\n"
".SH  FILES\n"
"\n"
".TP 15\n"
".B "FIM_CNS_DOC_PATH"/"FIM_CNS_FIM_TXT"\n"
"The\n"
".B Fim\n"
"documentation files (may have been installed in a different location than "FIM_CNS_DOC_PATH", in a custom install).\n"
".TP 15\n"
".B "FIM_CNS_SYS_RC_FILEPATH"\n"
"The system wide\n"
".B Fim\n"
"initialization file (executed at startup, after executing the hardcoded configuration).\n"

".TP 15\n"
".B "FIM_CNS_USR_RC_COMPLETE_FILEPATH"\n"
"The personal\n"
".B Fim\n"
"initialization file (executed at startup, after the system wide initialization file).\n"

".TP 15\n"
".B ~/.inputrc\n"
"If\n.B Fim\n"
"is built with GNU readline support, it will be susceptible to chages in the user set ~/.inputrc configuration file contents.  For details, see (man readline)."
"\n"

			      )+
string(
".SH SEE ALSO\n"
"Other \n"
".B Fim \n"
"man pages: fimgs(1), fimrc(1).\n"
".fi\n"
"Or related programs: fbset(1), con2fb(1), convert(1), vim(1), fb.modes(8), fbset(8), fbgrab(1), fbdev(4), setfont(8), xfs(1)\n"
".SH AUTHOR\n"
".nf\n"
FIM_AUTHOR" is the author of fim, \"fbi improved\". \n"
".fi\n"
FBI_AUTHOR" is the author of \"fbi\", upon which\n.B fim\nwas originally based. \n"
".SH COPYRIGHT\n"
".nf\n"
"Copyright (C) 2007-2011 "FIM_AUTHOR"\n"
".fi\n"
"Copyright (C) 1999-2004 "FBI_AUTHOR"\n"
".P\n"
"This program is free software; you can redistribute it and/or modify it under the terms of the GNU General Public License as published by the Free Software Foundation; either version 2 of the License, or (at your option) any later version.\n"
".P\n"
"This program is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more details.\n"
".P\n"
"You should have received a copy of the GNU General Public License along with this program; if not, write to the Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.\n"
)+
			string("\n");
	std::cout << mp;
	return 0;
}

int help_and_exit(char *argv0, int code=0, const char*helparg=NULL)
{
	if(helparg&&*helparg=='m')
	{
		fim_dump_man_page(); 
		goto done;
	}
	    cc.printHelpMessage(argv0);
	    std::cout << " where OPTIONS are taken from :\n";
	    if(helparg&&*helparg=='l') std::cout << "(EXPERIMENTAL: long help ('l') printout still unsupported)\n";
	    for(size_t i=0;i<fim_options_count-1;++i)
	    {	
		if(isascii(fim_options[i].val)){
	   	if((fim_options[i].val)!='-')std::cout << "\t-"<<(char)(fim_options[i].val) ;
	   	else std::cout << "\t-";}else std::cout<<"\t";
		std::cout << "\t\t";
	    	std::cout << "--"<<fim_options[i].name ;
		switch(fim_options[i].has_arg){
		case no_argument:
		break;
		case required_argument:
		//std::cout << " <arg>";
		if(fim_options[i].optdesc) std::cout << " =" << fim_options[i].optdesc; else std::cout << " =<arg>";
		break;
		case optional_argument:
		if(fim_options[i].optdesc) std::cout << " " << fim_options[i].optdesc; else std::cout << "[=arg]";
		break;
		default:
		;
		};
		if(helparg&&*helparg=='d')std::cout << "\t\t " << fim_options[i].desc;
		std::cout << FIM_SYM_ENDL;
		//if(helparg&&*helparg=='l') std::cout << "TODO: print extended help here\n";
		}
		std::cout << "\n Please read the documentation distributed with the program, in "FIM_CNS_FIM_TXT".\n"
			  << " For further help, consult the online help in fim (:"FIM_FLT_HELP"), and man fim (1), fimrc (1).\n"
			  << " For bug reporting please read the "FIM_CNS_BUGS_FILE" file.\n";
done:
	    std::exit(code);
	    return code;
}


	public:
	int main(int argc,char *argv[])
	{
		int retcode=0;
		/*
		 * an adapted version of the main function
		 * of the original version of the fbi program
		 */
		int              opt_index = 0;
		int              i;
		int		 want_random_shuffle=0;
	#ifdef FIM_READ_STDIN
		int              read_file_list_from_stdin;
		read_file_list_from_stdin=0;
		#ifdef FIM_READ_STDIN_IMAGE
		int		 read_one_file_from_stdin;
		read_one_file_from_stdin=0;
		#endif
		int		 read_one_script_file_from_stdin;
		read_one_script_file_from_stdin=0;
		int perform_sanity_check=0;
	#endif
		int c;
		int ndd=0;/*  on some systems, we get 'int dup(int)', declared with attribute warn_unused_result */
		bool appendedPostInitCommand=false;
	    	g_fim_output_device=FIM_CNS_EMPTY_STRING;
	
		setlocale(LC_ALL,"");	//uhm..

		{
			int foi;
			for(foi=0;foi<fim_options_count;++foi)
			{
				options[foi].name=fim_options[foi].name;
				options[foi].has_arg=fim_options[foi].has_arg;
				options[foi].flag=fim_options[foi].flag;
				options[foi].val=fim_options[foi].val;
			}
		}

	    	for (;;) {
		    /*c = getopt_long(argc, argv, "wc:u1evahPqVbpr:t:m:d:g:s:f:l:T:E:DNhF:",*/
		    c = getopt_long(argc, argv, "Ab?wc:uvahPqVr:m:d:g:s:T:E:f:DNhF:tfipW:o:S",
				options, &opt_index);
		if (c == -1)
		    break;
		switch (c) {
	/*	case 0:
		    // long option, nothing to do
		    break;*/
	//	case '1':
		    //fbi's
	//	    FIM_FPRINTF(stderr, "sorry, this feature will be implemented soon\n");
	//	    once = 1;
	//	    break;
		case 'a':
		    //fbi's
		    //cc.setVariable(FIM_VID_AUTOTOP,1);
		    //TODO: still needs some tricking .. 
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add("v:"FIM_VID_AUTO_SCALE_V"=1;");
		    cc.pre_autocmd_add(FIM_VID_AUTOWIDTH"=0;");/*  these mutual interactions are annoying */
	#else
		    cout << FIM_EMSG_NO_SCRIPTING;
	#endif
		    break;
		case 'b':
		    //fim's
		    if(optarg && strstr(optarg,"1")==optarg && !optarg[1])
			{
		    	cc.setVariable(FIM_VID_BINARY_DISPLAY,1);
			}
		    else
		    if(optarg && strstr(optarg,"24")==optarg && !optarg[2])
			{
		    	cc.setVariable(FIM_VID_BINARY_DISPLAY,24);
			}
                    else
		    {
			if(optarg)std::cerr<<"Warning : the --"FIM_OSW_BINARY" option supports 1 or 24 bpp depths. Using "<<FIM_DEFAULT_AS_BINARY_BPP<<".\n";
		    	cc.setVariable(FIM_VID_BINARY_DISPLAY,FIM_DEFAULT_AS_BINARY_BPP);
                    }
		    break;
		case 'A':
		    //fbi's
		    //cc.setVariable(FIM_VID_AUTOTOP,1);
		    //FIXME: still needs some tricking .. 
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add(FIM_VID_AUTOTOP"=1;");
	#endif
		    break;
		case 'q':
		    //fbi's
		    //FIM_FPRINTF(stderr, "sorry, this feature will be implemented soon\n");
		    //cc.setVariable(FIM_VID_DISPLAY_STATUS,0);
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add(FIM_VID_DISPLAY_STATUS"=0;");
	#endif
		    break;
		case 'f':
	#ifndef FIM_WANT_NOSCRIPTING
		    cc.setVariable(FIM_VID_DEFAULT_ETC_FIMRC,optarg);
	#else
		    cout << FIM_EMSG_NO_SCRIPTING;
	#endif
		    break;
		case 0x7373:
	#ifndef FIM_WANT_NOSCRIPTING
		    	cc.setVariable(FIM_VID_WANT_SLEEPS,optarg);
	    		cc.autocmd_add(FIM_ACM_PREEXECUTIONCYCLE,"",FIM_CNS_SLIDESHOW_CMD);
	#else
		    cout << FIM_EMSG_NO_SCRIPTING;
	#endif
		    break;
		case 'S':
		    //fim's
	#ifdef FIM_AUTOCMDS
		    cc.setVariable(FIM_VID_SANITY_CHECK,1);
		    perform_sanity_check=1;
	#endif
		    break;
		case 'v':
		    //fbi's
		    //cc.setVariable(FIM_VID_DISPLAY_STATUS,1);
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add(FIM_VID_DISPLAY_STATUS"=1;");
	#endif
		    break;
		case 'w':
		    //fbi's
		    //cc.setVariable(FIM_VID_AUTOWIDTH,1);
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add(FIM_VID_AUTOWIDTH"=1;");
	#endif
		    break;
		case 'P':
		    //fbi's
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add(FIM_VID_AUTOWIDTH"=1;"FIM_VID_AUTOTOP"=1;");
	#endif
		    break;
		case 0xFFD8FFE0:
		    //fbi's
	 	    // NEW
	#ifdef FIM_AUTOCMDS
		{
			int ipeppe_offset;

			ipeppe_offset=(int)atoi(optarg);
			if(ipeppe_offset<0)
				std::cerr<< "warning: ignoring user set negative offset value.\n";
			else
			if(ipeppe_offset>0)
			{
				string tmp;
				size_t peppe_offset=0;
				peppe_offset =(size_t)ipeppe_offset;
				tmp=FIM_VID_OPEN_OFFSET;
				tmp+="=";
				tmp+=string((int)peppe_offset);/* FIXME */
				tmp+=";";
				cc.pre_autocmd_add(tmp);
				//std::cout << "peppe_offset" << peppe_offset<< "\n";
			}
		}
	#endif
		    break;
		case 'g':
		    //fbi's
		    default_fbgamma = fim_atof(optarg);
		    break;
		case 'r':
		    cout << FIM_EMSG_UNFINISHED;
		    //fbi's
	// TODO
	//	    pcd_res = atoi(optarg);
		    break;
		case 's':
	//	    if(atoi(optarg)>0) cc.setVariable(FIM_VID_STEPS,atoi(optarg));
		    if(atoi(optarg)>0)
		    {
		    	// fixme : still buggy
		    	fim::string s=FIM_VID_STEPS;
			s+=fim::string((int)atoi(optarg));
			s+=";";
	#ifdef FIM_AUTOCMDS
			cc.pre_autocmd_add(s);
	#endif
		    }
		    break;
	//	case 't':
		    //fbi's
	//	    timeout = atoi(optarg);
	//	    FIM_FPRINTF(stderr, "sorry, this feature will be implemented soon\n");
	//	    break;
		case 'u':
		    //FIM_FPRINTF(stderr, "sorry, this feature will be implemented soon\n");
		    //fim's
		    want_random_shuffle=1;
		    break;
		case 'd':
		    //fbi's
		    default_fbdev = optarg;
		    break;
		case 'i':
		    //fim's
#ifdef FIM_READ_STDIN_IMAGE
		    read_one_file_from_stdin=1;
#else
		    FIM_FPRINTF(stderr, FIM_EMSG_NO_READ_STDIN_IMAGE);
#endif
		    break;
		case 'm':
		    //fbi's
		    default_fbmode = optarg;
		    break;
	//removed, editing features :
	/*	case 'f':
	//	    fontname = optarg;
		    break;
		case 'e':
	//	    editable = 1;
		    break;
		case 'b':
	//	    backup = 1;
		    break;
		case 'p':
	//	    preserve = 1;
		    break;*/
	//	case 'l':
		    //fbi's
	//	    flist_add_list(optarg);
	//	    FIM_FPRINTF(stderr, "sorry, this feature will be implemented soon\n");
	//	    break;
		case 'T':
		    //fbi's virtual terminal
		    default_vt = atoi(optarg);
		    break;
		case 'V':
		    version();
		    return 0;
		    break;
		case 'c':
		    //fim's
		    cc.appendPostInitCommand(optarg);
		    appendedPostInitCommand=true;
		    break;
		case 'W':
		    //fim's
		    cc.setVariable(FIM_VID_SCRIPTOUT_FILE,optarg);
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add(FIM_FLT_START_RECORDING";");
		    cc.appendPostExecutionCommand(FIM_FLT_STOP_RECORDING";");
	#endif

		    break;
		case 'F':
		    //fim's
		    cc.appendPostExecutionCommand(optarg);
		    break;
		case 'E':
		    //fim's
	#ifndef FIM_WANT_NOSCRIPTING
		    cc.push_scriptfile(optarg);
	#else
		    cout << FIM_EMSG_NO_SCRIPTING;
	#endif
		    break;
		case 'p':
		    //fim's (differing semantics from fbi's)
	#ifndef FIM_WANT_NOSCRIPTING
		    read_one_script_file_from_stdin=1;
	#else
		    cout << FIM_EMSG_NO_SCRIPTING;
	#endif
		    break;
		case 'D':
		    //fim's
	//	    cc.setNoFrameBuffer();	// no framebuffer (debug) mode
		    cc.dumpDefaultFimrc();
		    std::exit(0);
		    break;
		case 'N':
		    //fim's
			cc.setVariable(FIM_VID_NO_RC_FILE,1);
		    break;
		case 0x4E4E:// NN
		    //fim's
		    	cc.setVariable(FIM_VID_LOAD_DEFAULT_ETC_FIMRC,0);
		    break;
		case 't':
		    //fim's
			#ifdef FIM_WITH_AALIB
		    	g_fim_output_device=FIM_DDN_INN_AA;
			#else
			std::cerr << "you should recompile fim with aalib support!\n";
			g_fim_output_device=FIM_DDN_INN_DUMB;
			#endif
		    break;
		case 'o':
		    //fim's
		    	g_fim_output_device=optarg;
#if FIM_WANT_OUTPUT_DEVICE_STRING_CASE_INSENSITIVE
			transform(g_fim_output_device.begin(), g_fim_output_device.end(), g_fim_output_device.begin(),(int (*)(int))tolower);
#endif
		    break;
		case 0xd15cbab3:
		    //fim's
		{
			args_t args;
			if(optarg)args.push_back(optarg);
			cc.dump_reference_manual(args);
			std::exit(0);
		}
		    break;
	#ifdef FIM_READ_STDIN
		case '-':
		    //fim's
		    read_file_list_from_stdin=1;
		    break;
		case 0:
		    //fim's
		    read_file_list_from_stdin=1;
		    break;
	#endif
		default:
		case 'h':
		    help_and_exit(argv[0],0,optarg);
		}
	    }
		for (i = optind; i < argc; i++)
		{
	#ifdef FIM_READ_STDIN
			if(*argv[i]=='-'&&!argv[i][1])read_file_list_from_stdin=1;
			else
	#endif
			{
				cc.push(argv[i]);
			}
		}
	
		lexer=new yyFlexLexer;	//used by YYLEX
		if(!lexer)
		{
			FIM_FPRINTF(stderr, "error while allocating the lexer!\n\n");
			retcode=-1;
			goto ret;
		}

	#ifdef FIM_READ_STDIN	
		if( read_file_list_from_stdin +
		#ifdef FIM_READ_STDIN_IMAGE
		read_one_file_from_stdin+
		#endif
		read_one_script_file_from_stdin > 1)
		{
			FIM_FPRINTF(stderr, "error : you shouldn't specify more than one standard input reading options among (-, -p, ad -i)!\n\n");
			retcode=help_and_exit(argv[0],0);/* should return 0 or -1 ? */
			goto ret;
		}
		/*
		 * this is Vim's solution for stdin reading
		 * */
		if(read_file_list_from_stdin)
		{
			char *lineptr=NULL;
			size_t bs=0;
			while(fim_getline(&lineptr,&bs,stdin)>0)
			{
				chomp(lineptr);
				cc.push(lineptr);
				//printf("%s\n",lineptr);
				lineptr=NULL;
			}
			if(lineptr)fim_free(lineptr);
			close(0);
			ndd=dup(2);
		}
		#ifdef FIM_READ_STDIN_IMAGE
		else
		if(read_one_file_from_stdin)
		{
			/*
			 * we read a whole image file from stdin
			 * */
			FILE *tfd=NULL;
			if( ( tfd=fim_fread_tmpfile(stdin) )!=NULL )
			{	
				cc.fpush(tfd);
			}
			close(0);
			ndd=dup(2);
		}
		#endif
		else
		if(read_one_script_file_from_stdin)
		{
		    	char* buf;
			buf=slurp_binary_fd(0,NULL);
			if(buf) cc.appendPostInitCommand(buf);
			if(buf) appendedPostInitCommand=true;
			if(buf) fim_free(buf);
			close(0);
			ndd=dup(2);
		}
	#endif
		if(want_random_shuffle)
			cc.browser_._random_shuffle();

		if(ndd==-1)
			fim_perror(NULL);
	
		if(cc.browser_.empty_file_list()
#ifndef FIM_WANT_NOSCRIPTING
			       	&& !cc.with_scriptfile()
#endif
			       	&& !appendedPostInitCommand 
		#ifdef FIM_READ_STDIN_IMAGE
		&& !read_one_file_from_stdin
		#endif
		&& !perform_sanity_check
		)
		{retcode=help_and_exit(argv[0],-1);goto ret;}
	
		/* output device guess */
		if( g_fim_output_device==FIM_CNS_EMPTY_STRING )
		{
			#ifdef FIM_WITH_LIBSDL
			/* check to see if we are under X */
			if( fim_getenv(FIM_ENV_DISPLAY) )
			{
				g_fim_output_device=FIM_DDN_INN_SDL;
			}
			else
			#endif
#ifndef FIM_WITH_NO_FRAMEBUFFER
			g_fim_output_device=FIM_DDN_INN_FB;
#else
	#ifdef FIM_WITH_AALIB
			g_fim_output_device=FIM_DDN_INN_AA;
	#else
			g_fim_output_device=FIM_DDN_INN_DUMB ;
	#endif
#endif	//#ifndef FIM_WITH_NO_FRAMEBUFFER
		}

		// TODO : we still need a good output device probing mechanism

		if(cc.init(g_fim_output_device)!=0) {retcode=-1;goto ret;};
		retcode=cc.executionCycle();/* note that this could not return */
ret:
		return retcode;
	}

};

int main(int argc,char *argv[])
{
	/*
	 * FimInstance will encapsulate all of the fim's code someday.
	 * ...someday.
	 * */
	FimInstance fiminstance;
	Var::var_help_db_init();
	return fiminstance.main(argc,argv);
}

		/* FIXME: we are including files here.
		 * this is a horrible programming practice and shall be fixed. */
#ifdef FIM_WITH_LIBPNG
#include <png.h>
#endif
#ifdef HAVE_LIBJPEG
#include <jpeglib.h>
#endif
#ifdef FIM_HANDLE_TIFF
#include <tiffio.h>
#endif
#ifdef FIM_HANDLE_GIF
#include <gif_lib.h>
#endif
//#ifdef HAVE_LIBPOPPLER
//#include <poppler/PDFDoc.h> // getPDFMajorVersion getPDFMinorVersion
//#endif

	void FimInstance::version()
	{
	    FIM_FPRINTF(stderr, 
			    FIM_CNS_FIM" "
	#ifdef FIM_VERSION
			    FIM_VERSION
	#endif
	#ifdef SVN_REVISION
			    " ( repository version "
		SVN_REVISION
			    " )"
	#else
	/* obsolete */
	# define FIM_REPOSITORY_VERSION  "$LastChangedDate$"
	# ifdef FIM_REPOSITORY_VERSION 
			    " ( repository version "
		FIM_REPOSITORY_VERSION 	    
			    " )"
	# endif
	#endif
	#ifdef FIM_AUTHOR 
			    ", by "
			    FIM_AUTHOR
	#endif
			    ", built on %s\n",
			    __DATE__
	    		    " ( based on fbi version 1.31 (c) by 1999-2004 "FBI_AUTHOR_NAME" )\n"
	#ifdef FIM_WITH_LIBPNG
	#ifdef PNG_HEADER_VERSION_STRING 
	"Compiled with "PNG_HEADER_VERSION_STRING""
	#endif 
	#endif 
	#ifdef FIM_HANDLE_GIF
	#ifdef GIF_LIB_VERSION
	"Compiled with libgif, "GIF_LIB_VERSION".\n"
	#endif 
	#endif 
	#ifdef HAVE_LIBJPEG
	#ifdef JPEG_LIB_VERSION
	"Compiled with libjpeg, v."FIM_XSTRINGIFY(JPEG_LIB_VERSION)".\n"
	#endif 
	#endif 
	// for TIFF need TIFFGetVersion
	#ifdef CXXFLAGS
			"Compile flags: CXXFLAGS="CXXFLAGS
	#ifdef CFLAGS
			"  CFLAGS="CFLAGS
	#endif
			"\n"
	#endif
			"Fim options (features included (+) or not (-)):\n"
	#include "version.h"
	/* i think some flags are missing .. */
		"\nSupported output devices (for --"FIM_OSW_OUTPUT_DEVICE") : "
	#ifdef FIM_WITH_AALIB
		" "FIM_DDN_INN_AA
	#endif
	#ifdef FIM_WITH_CACALIB
		" "FIM_DDN_INN_CACA
	#endif
	#ifdef FIM_WITH_LIBSDL
		" "FIM_DDN_INN_SDL
	#endif
#ifndef FIM_WITH_NO_FRAMEBUFFER
		" "FIM_DDN_INN_FB
#endif //#ifndef FIM_WITH_NO_FRAMEBUFFER
	#if 1
		" "FIM_DDN_INN_DUMB
	#endif
		"\n"
		"\nSupported file formats : "
#ifdef ENABLE_PDF
		" pdf"
#endif
#ifdef HAVE_LIBSPECTRE
		" ps"
#endif
#ifdef HAVE_LIBDJVU
		" djvu"
#endif
#ifdef HAVE_LIBJPEG
		" jpeg"
#endif
#ifdef FIM_HANDLE_TIFF
		" tiff"
#endif
#ifdef FIM_HANDLE_GIF
		" gif"
#endif
#ifdef FIM_WITH_LIBPNG
		" png"
#endif
		" ppm"	/* no library is needed for these */
		" bmp"
#ifdef HAVE_MATRIX_MARKET_DECODER
		" mtx (Matrix Market)"
#endif
		"\n"
			    );
	}
	/* WARNING: PLEASE DO NOT WRITE ANY MORE CODE AFTER THIS DECLARATION (RIGHT ABOVE, AN UNCLEAN CODING PRACTICE WAS USED) */

