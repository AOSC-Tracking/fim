/* $Id$ */
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
}

struct fim_options_t{
  const char *name;
  int has_arg;
  int *flag;
  int val;
  const char *desc;/* this is fim specific */
  const char *optdesc;/* this is fim specific */
};

/*
 * Yet unfinished. 
 * This structure keeps hold of Fim's options flags.
 */
struct fim_options_t fim_options[] = {
    {"version",    no_argument,       NULL, 'V',"print program version",NULL},
    {"help",       optional_argument,       NULL, 'h',"print short (or descriptive, or long) program invocation help","[=s|d|l]"},
    {"device",     required_argument, NULL, 'd',"specify a {framebuffer device}","{framebuffer device}"},
    {"mode",       required_argument, NULL, 'm',"specify a video mode","{vmode}"},
    {FIM_OSW_BINARY,     optional_argument,       NULL, 'b',"view any file as either a 1 or 24 bpp bitmap","[=24|1]"},
    {"gamma",      required_argument, NULL, 'g',"set gamma","{gamma}"},
    {"quiet",      no_argument,       NULL, 'q',"quiet mode",NULL},
    {"verbose",    no_argument,       NULL, 'v',"verbose mode",NULL},
    {"scroll",     required_argument, NULL, 's',"set scroll value (in pixels)","{value}"},
/*    {"timeout",    required_argument, NULL, 't',"",NULL},*/  /* timeout value */	/* fbi's */
/*    {"once",       no_argument,       NULL, '1',"",NULL},*/  /* loop only once */
    {"resolution", required_argument, NULL, 'r',"set resolution (may not always work)","{resolution}"},
    {"random",     no_argument,       NULL, 'u',"randomize images order",NULL},
/*    {"font",       required_argument, NULL, 'f',"",NULL},*/  /* font */
    {"etc-fimrc",       required_argument, NULL, 'f',"etc-fimrc read (experimental)","{fimrc}"},
    {"autozoom",   no_argument,       NULL, 'a',"scale according to a best fit",NULL},
    {"autotop",   no_argument,       NULL, 'A',"align images to the top",NULL},
    {"autowidth",   no_argument,       NULL, 'w',"scale according to width",NULL},
/*    {"edit",       no_argument,       NULL, 'e',"",NULL},*/  /* enable editing */	/* fbi's */
/*    {"list",       required_argument, NULL, 'l',"",NULL},*/
    {"vt",         required_argument, NULL, 'T',"specify a virtual terminal for the framebufer","{terminal}"},
//    {"backup",     no_argument,       NULL, 'b',"",NULL},	/* fbi's */
    {FIM_OSW_EXECUTE_SCRIPT,   required_argument,       NULL, 'E',"execute {scriptfile} after initialization","{scriptfile}"},
    {FIM_OSW_EXECUTE_COMMANDS, required_argument,       NULL, 'c',"execute {commands} after initialization","{commands}"},
    {FIM_OSW_FINAL_COMMANDS,   required_argument,       NULL, 'F',"execute {commands} just before exit","{commands}"},
//    {"debug",      no_argument,       NULL, 'D',"",NULL},
    {"no-rc-file",      no_argument,       NULL, 'N',"do not read any configuration file at startup",NULL},
    {"dump-default-fimrc",      no_argument,       NULL, 'D',"dump on standard output the embedded configuration",NULL},
#ifdef FIM_READ_STDIN
    {"read-from-stdin",      no_argument,       NULL, '-',"read an image list from standard input",NULL},
#endif
    {"no-framebuffer",      no_argument,       NULL, 't',"display images in text mode (as -o aa)",NULL},
    {"text-reading",      no_argument,       NULL, 'P',"proceed scrolling as reading through a text document",NULL},
#ifdef FIM_READ_STDIN_IMAGE
    {"image-from-stdin",      no_argument,       NULL, 'i',"read an image file from standard input",NULL},
#endif
//    {"preserve",   no_argument,       NULL, 'p',"",NULL},	/* fbi's */
    {FIM_OSW_SCRIPT_FROM_STDIN,      no_argument,       NULL, 'p',"read commands from standard input",NULL},
    {"sanity-check",      no_argument,       NULL, 'S',"perform a sanity check",NULL},	/* NEW */
    {FIM_OSW_DUMP_SCRIPTOUT,      required_argument,       NULL, 'W',"will record any executed command to the a {scriptfile}","{scriptfile}"},
    {"offset",      required_argument,       NULL,  0xFFD8FFE0,"will open the first image file at the specified offset","{bytes-offset}"},/* NEW */
    {FIM_OSW_OUTPUT_DEVICE,      required_argument,       NULL, 'o',"specify using a specific output driver (if supported)","[fb|sdl|aa|dumb]"},
    {"dump-reference-help",      optional_argument /*no_argument*/,       NULL, 0xd15cbab3,"dump reference info","[=man]"},

    /* long-only options */
//    {"autoup",     no_argument,       &autoup,   1 },
//    {"autodown",   no_argument,       &autodown, 1 },
//    {"comments",   no_argument,       &comments, 1 },
    {0,0,0,0,0,0}
};

const int fim_options_count=sizeof(fim_options)/sizeof(fim_options_t);
struct option options[fim_options_count];


class FimInstance
{


	static void version()
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
	# define FIM_REPOSITORY_VERSION  "$Id$"
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
	    		    " ( based on fbi version 1.31 (c) by 1999-2003 Gerd Hoffmann )\n"
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

int help_and_exit(char *argv0, int code=0, const char*helparg=NULL)
{
	    cc.printHelpMessage(argv0);
	    std::cout << " where OPTIONS are taken from :\n";
	    if(helparg&&*helparg=='l') std::cout << "(EXPERIMENTAL: long help printout still unsupported)\n";
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
		std::cout << "\n Please read the documentation distributed with the program, in FIM.TXT.\n"
			  << " For further help, consult the online help in fim (:help), and man fim (1), fimrc (1).\n"
			  << " For bug reporting please read the BUGS file.\n";
	    std::exit(code);
	    return code;
}


	public:
	int main(int argc,char *argv[])
	{
		//char *default_fbdev=NULL,*default_fbmode=NULL;
		//int default_vt=-1;
		int retcode=0;
		//float default_fbgamma=-1.0;
		/*
		 * an adapted version of the main function
		 * of the original version of the fbi program
		 */
	// 	int              timeout = -1;	// fbi's
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
	//	char             *desc,*info;
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
		    c = getopt_long(argc, argv, "Ab?wc:uvahPqVr:m:d:g:s:T:E:DNhF:tfipW:o:S",
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
			if(optarg)std::cerr<<"Warning : the --"FIM_OSW_BINARY" option supports 1 or 24 bpp depths. Using 24.\n";
		    	cc.setVariable(FIM_VID_BINARY_DISPLAY,24);
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
		    cc.setVariable(FIM_VID_LOAD_DEFAULT_ETC_FIMRC,0);
		    /*
		     * note that this solution is temporary, because it clashes with -E (should have precedence, instead)
		     * */
	#ifndef FIM_WANT_NOSCRIPTING
		    cc.push_scriptfile(optarg);
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
				Image* stream_image=NULL;
				/*
				 * Note that it would be much nicer to do this in another way,
				 * but it would require to rewrite much of the file loading stuff
				 * (which is quite fbi's untouched stuff right now)
				 * */
				try{
					stream_image=new Image(FIM_STDIN_IMAGE_NAME,tfd);
				}catch (FimException e){/* write me */}

				// DANGEROUS TRICK!
				cc.browser_.set_default_image(stream_image);
				if(!cc.browser_.cache_.setAndCacheStdinCachedImage(stream_image))
					std::cerr << FIM_EMSG_CACHING_STDIN;// FIXME

				cc.browser_.push(FIM_STDIN_IMAGE_NAME);
				//fclose(tfd);	// uncommenting this will cause a segfault (why ? FIXME)
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

		if(cc.init(g_fim_output_device)!=0) return -1;
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


