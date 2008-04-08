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
#include <getopt.h>
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

/*
 * (nearly) all Fim stuff is in the fim namespace.
 * */
namespace fim
{
	/*
	 * Globals : should be encapsulated.
	 * */
	fim::FramebufferDevice ffd; 
	fim::CommandConsole cc(ffd);
}

/*
 * Yet unfinished. 
 * This structure keeps hold of Fim's options flags.
 */
struct option fim_options[] = {
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
    {"image-from-stdin",      no_argument,       NULL, 'i'},
    {"script-from-stdin",      no_argument,       NULL, 'p'},

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
			" "
	#ifdef FIM_BUGGED_CACHE
			"+FIM_BUGGED_CACHE"
	#else
			"-FIM_BUGGED_CACHE"
	#endif
			" "
	#ifdef FIM_AUTOSKIP_FAILED
			"+FIM_AUTOSKIP_FAILED"
	#else
			"-FIM_AUTOSKIP_FAILED"
	#endif
			" "
	#ifdef FIM_READ_STDIN
			"+FIM_READ_STDIN"
	#else
			"-FIM_READ_STDIN"
	#endif
			" "
	#ifdef FIM_BOZ_PATCH
			"+FIM_BOZ_PATCH"
	#else
			"-FIM_BOZ_PATCH"
	#endif
			    );
	}

int help_and_exit(char *argv0, int code=0)
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
	    std::exit(code);
	    return code;
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
		#ifdef FIM_READ_STDIN_IMAGE
		int		 read_one_file_from_stdin;
		read_one_file_from_stdin=0;
		#endif
		int		 read_one_script_file_from_stdin;
		read_one_script_file_from_stdin=0;
	#endif
	//	char             *desc,*info;
		int c;
		bool appendedPostInitCommand=false;
		Image* stream_image=NULL;
		g_fim_no_framebuffer=0;
	
		setlocale(LC_ALL,"");	//uhm..
	    	for (;;) {
		    /*c = getopt_long(argc, argv, "wc:u1evahPqVbpr:t:m:d:g:s:f:l:T:E:DNhF:",*/
		    c = getopt_long(argc, argv, "Awc:uvahPqVr:m:d:g:s:T:E:DNhF:tfip",
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
		    cc.setVariable("_load_default_etc_fimrc",0);
		    /*
		     * note that this solution is temporary, because it clashes with -E (should have precedence, instead)
		     * */
		    cc.push_scriptfile(optarg);
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
		case 'i':
		    //fim's
#ifdef FIM_READ_STDIN_IMAGE
		    read_one_file_from_stdin=1;
#else
		    fprintf(stderr,"sorry, the reading of images from stdin was disabled at compile time\n");
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
		    appendedPostInitCommand=true;
		    break;
		case 'F':
		    //fim's
		    cc.appendPostExecutionCommand(optarg);
		    break;
		case 'E':
		    //fim's
	#ifndef FIM_NOSCRIPTING
		    cc.push_scriptfile(optarg);
	#else
		    cout << "sorry, no scripting available!\n";
	#endif
		    break;
		case 'p':
		    //fim's (differing semantics from fbi's)
	#ifndef FIM_NOSCRIPTING
		    read_one_script_file_from_stdin=1;
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
		if( read_file_list_from_stdin +
		#ifdef FIM_READ_STDIN_IMAGE
		read_one_file_from_stdin+
		#endif
		read_one_script_file_from_stdin > 1)
		{
			fprintf(stderr,"error : you shouldn't specify more than one standard input reading options among (-, -p, ad -i)!\n\n");
			help_and_exit(argv[0],-1);
		}
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
		#ifdef FIM_READ_STDIN_IMAGE
		else
		if(read_one_file_from_stdin)
		{
			FILE *tfd=NULL;
			if( ( tfd=tmpfile() )!=NULL )
			{	
				char buf[1024];int rc=0;
				while( (rc=read(0,buf,1024))>0 ) fwrite(buf,rc,1,tfd);
				rewind(tfd);
				/*
				 * Note that it would be much nicer to do this in another way,
				 * but it would require to rewrite much of the file loading stuff
				 * (which is quite fbi's untouched stuff right now)
				 * */
				stream_image=new Image("/dev/stdout",tfd);

				// DANGEROUS TRICK!
				cc.browser.set_default_image(stream_image);
				cc.browser.push("");
			}
			//if(tfd)		fclose(tfd);
			close(0);
			dup(2);
		}
		#endif
		else
		if(read_one_script_file_from_stdin)
		{
		    	char* buf;
			buf=slurp_binary_fd(0,NULL);
			if(buf) cc.appendPostInitCommand(buf);
			if(buf) appendedPostInitCommand=true;
			if(buf) free(buf);
			close(0);
			dup(2);
		}
	#endif
	
	
		if(cc.browser.empty_file_list() && !cc.with_scriptfile() && !appendedPostInitCommand 
		#ifdef FIM_READ_STDIN_IMAGE
		&& !read_one_file_from_stdin
		#endif
		)
			help_and_exit(argv[0],-1);
	
		if((g_fim_no_framebuffer)==0)
		{
			if(default_fbdev)ffd.fbdev = default_fbdev;
			if(default_fbmode)ffd.fbmode = default_fbmode;
			if(default_vt!=-1)ffd.vt = default_vt;
			if(default_fbgamma!=-1.0)ffd.fbgamma = default_fbgamma ;
			if(ffd.framebuffer_init())cc.cleanup_and_exit(0);
			cc.tty_raw();// this, here, inhibits unwanted key printout (raw mode?!)
		}
		rl::initialize_readline(g_fim_no_framebuffer);
	
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
	 * FimInstance will encapsulate all of the fim's code someday.
	 * ...someday.
	 * */
	FimInstance fiminstance;
	return fiminstance.main(argc,argv);
}


