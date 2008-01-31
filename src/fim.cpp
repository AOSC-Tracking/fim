/* $Id$ */
/*
 fim.cpp : Fim main program and accessory functions

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

/*
 * This file contains mainly the code that couldn't fit in any of the existing classes.
 * When the fbi->fim transition will be complete, it will be probably very very small, 
 * as then all lone functions will be encapsulated.
 *
 * p.s.: it will be also _much_ cleaner ...
 * */

#include "fim.h"
#include <signal.h>
#include <sys/ioctl.h>
#include <getopt.h>

#include <linux/fb.h>
#include <linux/kd.h>

#include "readline.h"	/* readline stuff */

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
	int g_fim_no_framebuffer=1;
	FlexLexer *lexer;
//	using namespace fim;

/*
 * (nearly) all Fim stuff is in the fim namespace.
 * */
namespace fim
{
	/*
	 * Globals :/
	 * */
	fim::FramebufferDevice ffd; 
	fim::CommandConsole cc;
	static int fim_uninitialized = 1; // new

	struct termios  saved_attributes;
	int             saved_fl;

	/*
	 *	Setting the terminal in raw mode means:
	 *	 - setting the line discipline
	 *	 - setting the read rate
	 *	 - disabling the echo
	 */
	void tty_raw(void)
	{
		struct termios tattr;
		//we set the terminal in raw mode.
		    
		fcntl(0,F_GETFL,&saved_fl);
		tcgetattr (0, &saved_attributes);
		    
		//fcntl(0,F_SETFL,O_BLOCK);
		memcpy(&tattr,&saved_attributes,sizeof(struct termios));
		tattr.c_lflag &= ~(ICANON|ECHO);
		tattr.c_cc[VMIN] = 1;
		tattr.c_cc[VTIME] = 0;
		tcsetattr (0, TCSAFLUSH, &tattr);
	}
	
	void tty_restore()
	{	
		//POSIX.1 compliant:
		//"a SIGIO signal is sent whenever input or output becomes possible on that file descriptor"
		fcntl(0,F_SETFL,saved_fl);
		//the Terminal Console State Attributes will be set right NOW
		tcsetattr (0, TCSANOW, &saved_attributes);
	}

	/*
	 * This routine terminates the program as cleanly as possible.
	 * It should be used whenever useful.
	 */
	void cleanup_and_exit(int code)
	{
		if(g_fim_no_framebuffer)
		{
		//	tty_restore();
			std::exit(code);
		}
		else
		{
			ffd.fb_clear_mem();
			tty_restore();
			ffd.fb_cleanup();
			std::exit(code);
		}
	}

/*
 *	Set the 'status bar' of the program.
 *	- desc will be placed on the left corner
 *	- info on the right
 *	pointers are not freed
 *
 *	dez's
 */
void status(const char *desc, const char *info)
{
	//FIX ME : does this function always draw ?
	int chars, ilen;
	char *str,*p;
	const char *prompt=cc.get_prompt();
	char no_prompt[1];*no_prompt='\0';

	if(g_fim_no_framebuffer || fim_uninitialized)
		return;

	if(!cc.inConsole())prompt=no_prompt;
	chars = ffd.fb_var.xres / ffd.fb_font_width();
	if(chars<48)return;//something strange..
	str = (char*) malloc(chars+1);//this malloc is free
	if(!str)return;
	if (info)
	{
		ilen = strlen(info);
		sprintf(str, "%s%-*.*s [ %s ] H - Help",prompt,
		chars-14-ilen, chars-14-ilen, desc, info);//here above there is the need of 14+ilen chars
	}
	else
	{
		sprintf(str, "%s%-*.*s | H - Help",prompt, chars-11, chars-11, desc);
	}
	static int statusline_cursor;
	statusline_cursor=rl_point+1;
    
	if( statusline_cursor < chars && cc.inConsole()  ) str[statusline_cursor]='_';
	p=str-1;while(++p && *p)if(*p=='\n')*p=' ';

	ffd.fb_status_line((unsigned char*)str);
	free(str);
}

/*
 *	dez's
 */
void set_status_bar(fim::string desc, const char *info)
{
	set_status_bar(desc.c_str(), info);
}

/*
 *	dez's
 */
void set_status_bar(const char *desc, const char *info)
{
	/*
	 * pointers are not freed, by any means
	 */
	status(desc,info);
}

/*
 *	This function treats the framebuffer screen as a text outout terminal.
 *	So it prints all the contents of its buffer on screen..
 *	if noDraw is set, the screen will be not refreshed.
	 *	NULL,NULL is the clearing combination !!
	//FIX ME
	20070628 now this function adapts to the screen resolution. yet there happens 
	something strange for a number of lines filling more than half of the screen.. 

	dez's
 */
static void fb_status_screen(const char *msg)//, int noDraw=1)
{	
	/*	WARNING		*/
	//noDraw=0;
	/*	WARNING		*/
	if(g_fim_no_framebuffer)
	{
		if(msg)printf("%s",msg);
		return;
	}
	else
	{
	}
	int y,i,j,l,w;
	// R rows, C columns
	int R=(ffd.fb_var.yres/ffd.fb_font_height())/2,/* half screen : more seems evil */
	C=(ffd.fb_var.xres/ffd.fb_font_width());
	static char **columns=NULL;
	static char *columns_data=NULL;
	if(R<1 || C < 1)return;		/* sa finimm'acca', nun ce sta nient'a fa! */
	/* R rows and C columns; the last one for string terminators..
	 */
	if(!columns)columns=(char**)calloc(sizeof(char**)*R,1);
	if(!columns_data)columns_data=(char*)calloc(sizeof(char)*(R*(C+1)),1);
	/* 
	 * seems tricky : we allocate one single buffer and use it as console 
	 * storage and console pointers storage ...
	 *
	 * note that we don't deallocate this area until program termination.
	 * it is because we keep the framebuffer...
	 * */
	if(!columns || !columns_data)return;

	for(i=0;i<R;++i)columns[i]=columns_data+i*(C+1);

	static int cline=0,	//current line		[0..R-1]
		   ccol=0;	//current column	[0..C]
	const char *p=msg,	//p points to the substring not yet printed
	      	    *s=p;	//s advances and updates p

	if(!msg)
	{
		cline=0;
		ccol=0;
		p=NULL;
		/*noDraw=0;*/
	}
	if(msg&&*msg=='\0')return;

	if( g_fim_no_framebuffer )return;
	if(p)while(*p)
	{
	    //while there are characters to put on screen, we advance
	    while(*s && *s!='\n')++s;
	    //now s points to an endline or a NUL
	    l=s-p;
	    //l is the number of characters which should go on screen (from *p to s[-1])
	    w=0;
	    while(l>0)	//line processing
	    {
		    //w is the number of writable characters on this line ( w in [0,C-ccol] )
		    w=min(C-ccol,l);
		    //there remains l-=w non '\n' characters yet to process in the first substring
		    l-=w;
		    //we place the characters on the line (not padded,though)
		    strncpy(columns[cline]+ccol,p,w);
		    sanitize_string_from_nongraph(columns[cline]+ccol,w);
		    //the current column index is updated,too
		    ccol+=w;
		    //we blank the rest of the line (SHOULD BE UNNECESSARY)
		    for(i=ccol;i<C;++i)columns[cline][i]=' ';
		    //we terminate the line with a NUL
		    columns[cline][C]='\0';
		    //please note that ccol could still point to the middle of the line
		    //the last writable column index is C
		    if(ccol>=C+1)fim::cleanup_and_exit(-1);	//ehm.. who knows
		    if(ccol==C)
		    {
			    //So if we are at the end of the line, we prepare 
			    //for a new line
			    ccol=0;
			    cline=(cline+1)%(R);
			    if(cline==0)
			    for(i=0;i<R;++i)
			    {
				    for(j=0;j<C;++j)columns[i][j]=' ';
				    columns[i][C]='\0';
			    }
			    //we clean the new line (SHOULD BE NECESSARY ONLY WITH THE FIRST LINE!)
		    	    for(i=0;i<C;++i)columns[cline][i]=' ';
		    }
	            //we advance in the string for w chars 
	    	    p+=w;	//a temporary assignment
	    }
	    	/*
		 * after the chars in [p,s-1] are consumed, we can continue
		 */
		    while(*s=='\n')
		    {
			    ++s;
			    ccol=0;
			    cline=(cline+1)%(R);
			    if(cline==0)
			    for(i=0;i<R;++i)
			    {
				    for(j=0;j<C;++j)columns[i][j]=' ';
				    columns[i][C]='\0';
			    }
		    }
	    p=s;
	}

	//if(!cc.drawOutput() || noDraw)return;//CONVENTION!
	if(!cc.drawOutput() )return;//CONVENTION!

	    y = 1*ffd.fb_font_height();
	    for(i=0  ;i<R ;++i) ffd.fs_puts(ffd.fb_font_get_current_font(), 0, y*(i), (unsigned char*)columns[i]);

	    /*
	     *WARNING : note that columns and columns_data arrays are not freed and should not, as long as they are static.
	     * */
}

/*
 * sets the status bar of the screen to the specified strings:
 *  desc on the left corner
 *  info on the right corner
 *  FIXME : actually, info is ignored
 */
void status_screen(const char *desc, char *info)
{
	if(g_fim_no_framebuffer)return;
	/*
	 *	TO FIX
	 *	NULL,NULL is the clearing combination !!
	 */
	//if(!desc)return;	// !!
	fb_status_screen(desc);
}

}

/*
 * Yet unfinished. 
 * This structure keeps hold of Fim's options flags.
 */
static struct option fim_options[] = {
    {"version",    no_argument,       NULL, 'V'},  /* version */
    {"help",       no_argument,       NULL, 'h'},  /* help */
    {"device",     required_argument, NULL, 'd'},  /* device */
    {"mode",       required_argument, NULL, 'm'},  /* video mode */
    {"gamma",      required_argument, NULL, 'g'},  /* set gamma */
    {"quiet",      no_argument,       NULL, 'q'},  /* quiet */
    {"verbose",    no_argument,       NULL, 'v'},  /* verbose */
    {"scroll",     required_argument, NULL, 's'},  /* set scrool */
/*    {"timeout",    required_argument, NULL, 't'},*/  /* timeout value */
/*    {"once",       no_argument,       NULL, '1'},*/  /* loop only once */
    {"resolution", required_argument, NULL, 'r'},  /* select resolution */
    {"random",     no_argument,       NULL, 'u'},  /* randomize images */
/*    {"font",       required_argument, NULL, 'f'},*/  /* font */
    {"etc-fimrc",       required_argument, NULL, 'f'},  /* etc-fimrc read (experimental) */
    {"autozoom",   no_argument,       NULL, 'a'},
    {"autotop",   no_argument,       NULL, 'A'},
    {"autowidth",   no_argument,       NULL, 'w'},
/*    {"edit",       no_argument,       NULL, 'e'},*/  /* enable editing */
/*    {"list",       required_argument, NULL, 'l'},*/
    {"vt",         required_argument, NULL, 'T'},
//    {"backup",     no_argument,       NULL, 'b'},
//    {"preserve",   no_argument,       NULL, 'p'},
    {"execute-script",   required_argument,       NULL, 'E'},
    {"execute-commands", required_argument,       NULL, 'c'},
    {"final-commands",   required_argument,       NULL, 'F'},
//    {"debug",      no_argument,       NULL, 'D'},
    {"no-rc-file",      no_argument,       NULL, 'N'},
    {"dump-default-fimrc",      no_argument,       NULL, 'D'},
#ifdef FIM_READ_STDIN
    {"read-from-stdin",      no_argument,       NULL, '-'},
#endif
    {"no-framebuffer",      no_argument,       NULL, 't'},
    {"text-reading",      no_argument,       NULL, 'P'},

    /* long-only options */
//    {"autoup",     no_argument,       &autoup,   1 },
//    {"autodown",   no_argument,       &autodown, 1 },
//    {"comments",   no_argument,       &comments, 1 },
    {0,0,0,0}
};



class FimInstance
{
	static void version()
	{
	    fprintf(stderr,
			    "FIM - Fbi IMproved "
	#ifdef FIM_VERSION
			    FIM_VERSION
	#endif
	#ifdef FIM_REPOSITORY_VERSION 
			    "( repository version "
		FIM_REPOSITORY_VERSION 	    
			    " )"
	#endif
	#define FIM_AUTHOR "Michele Martone <dezperado _CUT_ autistici _CUT_ org>"
	#ifdef FIM_AUTHOR 
			    ", by "
			    FIM_AUTHOR
	#endif
			    ", built on %s\n",
			    __DATE__
	    		    " ( based on fbi version 1.31 (c) by 1999-2003 Gerd Hoffmann )\n"
			"Compile flags:\n"
	#ifdef FIM_WINDOWS
			"+FIM_WINDOWS"
	#else
			"-FIM_WINDOWS"
	#endif
			" "
	#ifdef FIM_DEFAULT_KEY_CONFIG  
			"+FIM_DEFAULT_KEY_CONFIG"
	#else
			"-FIM_DEFAULT_KEY_CONFIG"
	#endif
			" "
	#ifdef FIM_DEFAULT_CONFIG      
			"+FIM_DEFAULT_CONFIG"
	#else
			"-FIM_DEFAULT_CONFIG"
	#endif
			" "
	#ifdef FIM_DEFAULT_CONFIGURATION 
			"+FIM_DEFAULT_CONFIGURATION"
	#else
			"-FIM_DEFAULT_CONFIGURATION"
	#endif
			" "
	#ifdef FIM_NOFIMRC
			"+FIM_NOFIMRC"
	#else
			"-FIM_NOFIMRC"
	#endif
			" "
	#ifdef FIM_AUTOCMDS
			"+FIM_AUTOCMDS"
	#else
			"-FIM_AUTOCMDS"
	#endif
			    );
	}

int help_and_exit(char *argv0)
{
	    cc.printHelpMessage(argv0);
	    std::cout << " where OPTIONS are taken from :\n";
	    for(int i=0;((unsigned int)i)<(sizeof(fim_options)/sizeof(struct option))-1;++i)
	    {	
	   	if((fim_options[i].val)!='-')std::cout << "\t-"<<(char)(fim_options[i].val) ;
	   	else std::cout << "\t-";
		std::cout << "\t\t";
	    	std::cout << "--"<<fim_options[i].name ;
		switch(fim_options[i].has_arg){
		case no_argument:
		break;
		case required_argument:
		std::cout << " <arg>";
		break;
		default:
		;
		};
		std::cout << "\n";
		}
		std::cout << " ( Please read the documentation distributed with the program, too, in FIM.TXT)\n";
	    std::exit(0);
	    return 0;
}


	public:
	int main(int argc,char *argv[])
	{
		char *default_fbdev=NULL,*default_fbmode=NULL;
		int default_vt=-1;
		float default_fbgamma=-1.0;
		/*
		 * an adapted version of the main function
		 * of the original version of the fbi program
		 */
	// 	int              timeout = -1;
		int              opt_index = 0;
		int              i;
	#ifdef FIM_READ_STDIN
		int              read_file_list_from_stdin;
		read_file_list_from_stdin=0;
	#endif
	//	char             *desc,*info;
		char c;
		g_fim_no_framebuffer=0;
	
		setlocale(LC_ALL,"");	//uhm..
	    	for (;;) {
		    /*c = getopt_long(argc, argv, "wc:u1evahPqVbpr:t:m:d:g:s:f:l:T:E:DNhF:",*/
		    c = getopt_long(argc, argv, "Awc:uvahPqVr:m:d:g:s:T:E:DNhF:tf",
				fim_options, &opt_index);
		if (c == -1)
		    break;
		switch (c) {
	/*	case 0:
		    // long option, nothing to do
		    break;*/
	//	case '1':
		    //fbi's
	//	    fprintf(stderr,"sorry, this feature will be implemented soon\n");
	//	    once = 1;
	//	    break;
		case 'a':
		    //fbi's
		    //cc.setVariable("autotop",1);
		    //FIXME: still needs some tricking .. 
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add("auto_scale_v=1;");
	#endif
		    break;
		case 'A':
		    //fbi's
		    //cc.setVariable("autotop",1);
		    //FIXME: still needs some tricking .. 
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add("autotop=1;");
	#endif
		    break;
		case 'q':
		    //fbi's
		    //fprintf(stderr,"sorry, this feature will be implemented soon\n");
		    //cc.setVariable("_display_status",0);
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add("_display_status=0;");
	#endif
		    break;
		case 'f':
		    cc.setVariable("_load_default_etc_fimrc",1);
		    break;
		case 'v':
		    //fbi's
		    //cc.setVariable("_display_status",1);
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add("_display_status=1;");
	#endif
		    break;
		case 'w':
		    //fbi's
		    //cc.setVariable("autowidth",1);
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add("autowidth=1;");
	#endif
		    break;
		case 'P':
		    //fbi's
		    //FIXME
	//	    cc.setVariable("autowidth",1);
	//	    cc.setVariable("autotop",1);
	//	    strange : if the assignations occur in two pre_autocmd_add calls, it triggers a bug via fimgs:
	#ifdef FIM_AUTOCMDS
		    cc.pre_autocmd_add("autowidth=1;autotop=1;");
	#endif
		    break;
		case 'g':
		    //fbi's
		    default_fbgamma = atof(optarg);
		    break;
		case 'r':
		    //fbi's
	//	    pcd_res = atoi(optarg);
		    break;
		case 's':
	//	    if(atoi(optarg)>0) cc.setVariable("steps",atoi(optarg));
		    if(atoi(optarg)>0)
		    {
		    	// fixme : still buggy
		    	fim::string s="steps=";
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
	//	    fprintf(stderr,"sorry, this feature will be implemented soon\n");
	//	    break;
		case 'u':
		    //fbi's
		    fprintf(stderr,"sorry, this feature will be implemented soon\n");
	//	    randomize = 1;
		    break;
		case 'd':
		    //fbi's
		    default_fbdev = optarg;
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
	//	    fprintf(stderr,"sorry, this feature will be implemented soon\n");
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
		    break;
		case 'F':
		    //fim's
		    cc.appendPostExecutionCommand(optarg);
		    break;
		case 'E':
		    //fim's
	#ifndef FIM_NOSCRIPTING
		    cc.push_script(optarg);
	#else
		    cout << "sorry, no scripting available!\n";
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
			cc.setVariable("_no_rc_file",1);
		    break;
		case 't':
		    //fim's
		    	g_fim_no_framebuffer=1;
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
		    help_and_exit(argv[0]);
	#if 0
		    cc.printHelpMessage(argv[0]);
		    std::cout << " where OPTIONS are taken from :\n";
		    for(i=0;((unsigned int)i)<(sizeof(fim_options)/sizeof(struct option))-1;++i)
		    {	
		   	if((fim_options[i].val)!='-')std::cout << "\t-"<<(char)(fim_options[i].val) ;
		   	else std::cout << "\t-";
			std::cout << "\t\t";
		    	std::cout << "--"<<fim_options[i].name ;
			switch(fim_options[i].has_arg){
			case no_argument:
			break;
			case required_argument:
			std::cout << " <arg>";
			break;
			default:
			;
			};
			std::cout << "\n";
			}
			std::cout << " ( Please read the documentation distributed with the program, too, in FIM.TXT)\n";
		    std::exit(0);
	#endif
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
	
	#ifdef FIM_READ_STDIN
		
		/*
		 * this is Vim's solution for stdin reading
		 * */
		if(read_file_list_from_stdin)
		{
			char *lineptr=NULL;
			size_t bs=0;
			while(getline(&lineptr,&bs,stdin)>0)
			{
				chomp(lineptr);
				cc.push(lineptr);
				//printf("%s\n",lineptr);
				free(lineptr);
				lineptr=NULL;
			}
			close(0);
			dup(2);
		}
	#endif
	
	
		if(cc.browser.empty_file_list())
			help_and_exit(argv[0]);
	
		if((g_fim_no_framebuffer)==0)
		{
			if(default_fbdev)ffd.fbdev = default_fbdev;
			if(default_fbmode)ffd.fbmode = default_fbmode;
			if(default_vt!=-1)ffd.vt = default_vt;
			if(default_fbgamma!=-1.0)ffd.fbgamma = default_fbgamma ;
			if(ffd.framebuffer_init())cleanup_and_exit(0);
			tty_raw(); // this, here, inhibits unwanted key printout (raw mode?!)
		}
		rl::initialize_readline();
	
		fim_uninitialized = 0; // new
		if(cc.init()!=0) return -1;
	
	//	ffd.test_drawing();
		//while(1);
		//while(1){int i;for(i=0;i<500000;++i)ffd.fb_mem[i]=200;}
		//while(1)ffd.fb_rect(1,100,1,100);
		//cleanup_and_exit(0);
	
		cc.executionCycle();
		return 0;
	}

};

int main(int argc,char *argv[])
{
	/*
	 * FimInstance will contain all of the fim's code someday.
	 * ...someday.
	 * */
	FimInstance fiminstance;
	return fiminstance.main(argc,argv);
}


