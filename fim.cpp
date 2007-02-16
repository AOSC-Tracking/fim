
#include "fim.h"
#include <signal.h>
#include <sys/ioctl.h>
#include <getopt.h>

#include <linux/fb.h>
#include <linux/kd.h>
#include <linux/vt.h>
#include "src/fbtools.h"


//#define DEFAULT_DEVICE  "/dev/fb0"
#define DEFAULT_DEVICE  "/dev/fb/0"

/*
 * we use a portion of the STL
 */
using std :: endl;
using std :: ifstream;
using std :: ofstream;
using std :: map;
using std :: multimap;
using std :: pair;
using std :: vector;

class CommandConsole;

char * command_generator (const char *text,int state);

char * dupstr (const char* s)
{
	//allocation and duplication of a single string
	char *r = (char*) malloc (strlen (s) + 1);
	//FIX ME
	if(!r){assert(r);cc.quit();}
	strcpy (r, s);
	return (r);
}

namespace fim
{
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
		fcntl(0,F_SETFL,saved_fl);
		tcsetattr (0, TCSANOW, &saved_attributes);
	}

	void cleanup_and_exit(int code)
	{
	#ifndef FIM_NOFB
		fb_clear_mem();
		tty_restore();
		fb_cleanup();
	#endif
		exit(code);
	}
};

void status(const char *desc, const char *info)
{
	//FIX ME
	int chars, ilen;
	char *str,*p;
#ifndef FIM_NOFB
//	if (!statusline)return;
	chars = fb_var.xres / fb_font_width();
	str = (char*) malloc(chars+1);
	if(!str)return;
	if (info)
	{
		ilen = strlen(info);
		sprintf(str, "%-*.*s [ %s ] H - Help",
		chars-14-ilen, chars-14-ilen, desc, info);
	}
	else
	{
		sprintf(str, "%-*.*s | H - Help", chars-11, chars-11, desc);
	}
	extern int rl_point;
	static int statusline_cursor;
	statusline_cursor=rl_point;
    
	if( statusline_cursor < chars && cc.inConsole()  ) str[statusline_cursor]='_';
	p=str-1;while(++p && *p)if(*p=='\n')*p=' ';

	fb_status_line((unsigned char*)str);
	free(str);
#endif
}

char *make_info(struct ida_image *img, float scale)
{
	//FIX ME
	static char linebuffer[128];
	snprintf(linebuffer, sizeof(linebuffer),
	     "%s%.0f%% %dx%d %d/%d",
	     /*fcurrent->tag*/ 0 ? "* " : "",
	     scale*100,
	     img->i.width, img->i.height,
	     cc.current_image(),
	     cc.current_images()
	     );
	return linebuffer;
}

void fb_status_screen(const char *msg, int noDraw=1)
{
	//FIX ME
	if(!msg) return;
	if(cc.noFrameBuffer())return;
#ifndef FIM_NOFB
	/*
	 * This function treats the framebuffer screen as a terminal
	 */

	int y,i,j,l,w;
	//TODO : VARIABLES HERE!!
#define R 24
//#define R 30
//#define C 80
#define C 120+4
	static char columns[R][C+1];//columns[:][C]='\0'
	static int cline=0,	//current line		[0..R-1]
		   ccol=0;	//current column	[0..C]
	const char *p=msg,	//p points to the substring not yet printed
	      	    *s=p;	//s advances and updates p
	while(*p)
	{
	    //while there are characters to put on screen, we advance
	    while(*s && *s!='\n')++s;
	    //now s points to an endline or a NUL
	    l=s-p;
	    //l is the number of characters which should go on screen (from *p to s[-1])
	    w=0;
	    while(l>0)	//line processing
	    {
		    if(0)
		    {
			    //clear the current line
			    for(i=ccol;i<C;++i)columns[cline][i]=' ';
			
			    //clear all the lines after
			    for(i=cline+1;i<R;++i)
			    {
				    for(j=0;j<C;++j)columns[i][j]=' ';
				    columns[i][C]='\0';
			    }
		    
		    }
		    //w is the number of writable characters on this line ( w in [0,C-ccol] )
		    w=min(C-ccol,l);
		    //there remains l-=w non '\n' characters yet to process in the first substring
		    l-=w;
		    //we place the characters on the line (not padded,though)
		    strncpy(columns[cline]+ccol,p,w);
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
	if(!cc.drawOutput() || noDraw)return;//CONVENTION!
//	    if (!visible) return;
//	    y = fb_var.yres - f->height - ys;
//	    y = 1*f->height;
	    y = 1*fb_font_height();
//	    fb_memset(fb_mem + fb_fix.line_length * y, 0,
//	    fb_fix.line_length * (f->height+ys));
//	    fb_line(0, fb_var.xres, y, y);
	//    cline=0; strcpy(columns[0],"gaba");
//	    for(i=cline;i>=0;--i) fs_puts(f, 0, y*(i+1), columns[i]);
	    for(i=R-1;i>=0;--i) fs_puts(fb_font_get_current_font(), 0, y*(i), (unsigned char*)columns[i]);
//	    for(i=cline;i>=0;--i) fs_puts(f, 0, y*(i+1), "foo");
//	    for(i=0;i>=0;--i) fs_puts(f, 0, y*(i+1), "foo");
//    fs_puts(f, 0, y+ys, msg);
#undef R 
#undef C 
#else
	printf("%s",msg);
	return;
#endif
}

void status_screen(const char *desc, char *info)
{
#ifndef FIM_NOFB
	/*
	 *	TO FIX
	 */
	int chars, ilen;
	char *str;
	if(!desc)return;
	fb_status_screen(desc);
    	return;
//    if (!statusline) return;
	chars = fb_var.xres / fb_font_width();
	str = (char*) malloc(chars+1);
	assert(str);
/*    if (info) {
	ilen = strlen(info);
	sprintf(str, "%-*.*s [ %s ] H - Help",
		chars-14-ilen, chars-14-ilen, desc, info);
    } else*/ {
//	sprintf(str, "%-*.*s | H - Help", chars-11, chars-11, desc);
//	sprintf(str, "%s",  desc);
    	}
    //fb_status_screen(str);
	free(str);
#endif
}

namespace rl
{
/* 
 * Attempt to complete on the contents of TEXT.  START and END
 *     bound the region of rl_line_buffer that contains the word to
 *     complete.  TEXT is the word to complete.  We can use the entire
 *     contents of rl_line_buffer in case we want to do some simple
 *     parsing.  Return the array of matches, or NULL if there aren't any.
 */
char ** fim_completion (const char *text, int start,int end)
{
	//FIX ME
	char **matches;
	regex_t blank_regex;	//should be static!!!
	const int nmatch=1;
	regmatch_t pmatch[nmatch];
	char bc;
	if(regcomp(&blank_regex,"^ \\+$", 0)==-1)
	{
//		std::cout << "error calling regcomp!" << "\n";
	}
	else
	{
//		std::cout << "done calling regcomp!" << "\n";
	}
	if(regexec(&blank_regex,rl_line_buffer+start,nmatch,pmatch,0)==0)
	{
//		std::cout << "\"";
//		for(int m=pmatch[0].rm_so;m<pmatch[0].rm_eo;++m)
//			std::cout << rl_line_buffer[start+m];
//		std::cout << "\"\n";
	}
	else
	{
//		std::cout << "no match" << "\n";
	};
	regfree(&blank_regex);
	       
        matches = (char **)NULL;

            /* If this word is at the start of the line, then it is a command
	     *  to complete.  Otherwise it is the name of a file in the current
	     *  directory.
	     */
        if (start == 0)
	{
		//std::cout << "completion for word " << start << "\n";
		matches = rl_completion_matches (text, command_generator);
	}
	else 
	{
		//std::cout << "sorry, no completion for word " << start << "\n";
	}
        return (matches);
}

/*
 * 	this function is called to display the proposed autocompletions
 */
void completion_display_matches_hook(char **matches,int num,int max)
{
	//FIX ME
	//rl_display_match_list(matches,num,max);
	char buffer[256];
	int w,f,l;w=0;f=sizeof(buffer)-1;l=0;
	buffer[0]='\0';
	if(!matches)return;
	//return;
	for(int i=0;i<num && matches[i] && f>0;++i)
	{
		w=min(strlen(matches[i])+1,f);
		if(f>0){
		strncpy(buffer+l,matches[i],w);
		w=strlen(buffer+l);l+=w;f-=w;}
		if(f>0){strcpy(buffer+l," ");--f;++l;}
		buffer[l]='\0';
//		strcpy(buffer+strlen(buffer),matches[i]);
//		strcpy(buffer+strlen(buffer)," ");
	}
	//      status_screen((unsigned char*)buffer, NULL);
	cout << buffer << "\n" ;
//	std::cout << buffer << "\n" ;
 //     status((unsigned char*)"here shall be autocompletions", NULL);
}

void redisplay()
{	
	//FIX ME
	static int c=100;
//    fb_setcolor(c=~c);//sleep(1);
#ifndef FIM_NOFB
	status(( char*)rl_line_buffer,NULL);
#else
	printf("%s",rl_line_buffer);
#endif
//	fprintf(stderr,"::%s\n",rl_line_buffer);
//	fprintf(stdout,"::%s\n",rl_line_buffer);
}

int redisplay_hook()
{
	redisplay();
}

/*
 *	initial setup to set the readline library working
 */
void initialize_readline ()
{
	//FIX ME
	/* Allow conditional parsing of the ~/.inputrc file. */
	rl_readline_name = "fim";	//??
	/* Tell the completer that we want a crack first. */
	rl_attempted_completion_function = fim_completion;
	rl_completion_display_matches_hook=completion_display_matches_hook;
#ifndef FIM_NOFB
	rl_catch_signals=0;
	rl_catch_sigwinch=0;
	rl_redisplay_function=redisplay;
        rl_event_hook=redisplay_hook;
        rl_pre_input_hook=redisplay_hook;
#endif

/*	rl_voidfunc_t *rl_redisplay_function=redisplay;
	rl_hook_func_t *rl_event_hook=redisplay_hook;
	rl_hook_func_t *rl_pre_input_hook=redisplay_hook;*/
	//std::cout << "readline initialized\n";
}
};


namespace fim
{
	/*
	 *	Implementation of a dumb string class.
	 */
//class CommandConsoleCommand:public Command
//{
//	CommandConsole *obj;
//	fim::string (CommandConsole::*method)(std::vector<Arg>);
//	public:
/*	CommandConsoleCommand::CommandConsoleCommand(
			fim::string cmd,
			fim::string (CommandConsole::*method)(std::vector<Arg>),
			CommandConsole *obj,
			fim::string help)
	:Command(cmd,help),method(method),obj(obj)
	{}
	fim::string execute(const std::vector<Arg> &args)
	{
		std::cout << "dummy execution of " << cmd << ":\n";
		std::cout << obj << ":\n";
		if(!obj){std::cout<<"warning : null command pointer!\n";}
		else (obj->*method)(args);
	}*/
//};
};

fim::CommandConsole cc;

/* Generator function for command completion.  STATE lets us
 *    know whether to start from scratch; without any state
 *       (i.e. STATE == 0), then we start at the top of the list. */
char * command_generator (const char *text,int state)
{
//	static int list_index, len;
//	char *name;
	/* If this is a new word to complete, initialize now.  This
	 *      includes saving the length of TEXT for efficiency, and
	 *	initializing the index variable to 0. 
	 */
	return cc.command_generator(text,state);

		
//	if (!state) { list_index = 0; len = strlen (text); }

        /* Return the next name which partially matches from the
	 * command list.
	 */

//	while (name = commands[list_index].name)
//	{ list_index++; if (strncmp (name, text, len) == 0) return (dupstr(name)); }
	/* If no names matched, then return NULL. */
//	return ((char *)NULL);
}

#define TRUE            1
#define FALSE           0

void console_switch(int is_busy)
{
	//FIX ME
	switch (fb_switch_state) {
	case FB_REL_REQ:
		fb_switch_release();
	case FB_INACTIVE:
	visible = 0;///////
	break;
	case FB_ACQ_REQ:
		fb_switch_acquire();
	case FB_ACTIVE:
		visible = 1;	///////////
		redraw = 1;
		ioctl(fd,FBIOPAN_DISPLAY,&fb_var);
		/*
		 * PROBLEMS : //fb_clear_screen causes tearing!
		 */
		//fb_clear_screen();
	/*
	 * thanks to the next line, the image is redrawn each time 
	 * the console is switched!
	 */
		//cc.display();
//	if (is_busy) status("busy, please wait ...", NULL);		
	break;
	default:
	break;
    	}
	switch_last = fb_switch_state;
	return;
}



// FIX ME
static struct option fim_options[] = {
    {"version",    no_argument,       NULL, 'V'},  /* version */
    {"help",       no_argument,       NULL, 'h'},  /* help */
    {"device",     required_argument, NULL, 'd'},  /* device */
    {"mode",       required_argument, NULL, 'm'},  /* video mode */
    {"gamma",      required_argument, NULL, 'g'},  /* set gamma */
    {"quiet",      no_argument,       NULL, 'q'},  /* quiet */
    {"verbose",    no_argument,       NULL, 'v'},  /* verbose */
    {"scroll",     required_argument, NULL, 's'},  /* set scrool */
    {"timeout",    required_argument, NULL, 't'},  /* timeout value */
    {"once",       no_argument,       NULL, '1'},  /* loop only once */
    {"resolution", required_argument, NULL, 'r'},  /* select resolution */
    {"random",     no_argument,       NULL, 'u'},  /* randomize images */
    {"font",       required_argument, NULL, 'f'},  /* font */
    {"autozoom",   no_argument,       NULL, 'a'},
    {"edit",       no_argument,       NULL, 'e'},  /* enable editing */
    {"list",       required_argument, NULL, 'l'},
    {"vt",         required_argument, NULL, 'T'},
    {"backup",     no_argument,       NULL, 'b'},
    {"preserve",   no_argument,       NULL, 'p'},
    {"execute",    required_argument,       NULL, 'E'},
    {"debug",      no_argument,       NULL, 'D'},
    {"no-rc-file",      no_argument,       NULL, 'N'},

    /* long-only options */
//    {"autoup",     no_argument,       &autoup,   1 },
//    {"autodown",   no_argument,       &autodown, 1 },
//    {"comments",   no_argument,       &comments, 1 },
    {0,0,0,0}
};


	FlexLexer *lexer;
	using namespace fim;

void version()
{
	//FIX ME
#define FIM_VERSION "0.1"
    fprintf(stderr,
		    "fim version " FIM_VERSION", by dez ; compiled on %s.\n%s"
		    __DATE__,
    		    "\nbased on fbi version 1.31  (c) by 1999-2003 Gerd Knorr\n");
#undef FIM_VERSION
}

int main(int argc,char *argv[])
{
	/*
	 * an adapted version of the main function
	 * of the original version of the fbi program
	 */
	void *fp;
	//std::map<fim::string,fim::string (fim::CommandConsole::*)(std::vector<Arg>)> map;
	//m.insert(20,33);
	//m[20]=33;
	//std::cout<<m[20]<<"\n";
//	cc.bind('E',"comando E");
//	cc.bind('a'-'a'+1,"comando control e a");
//	
//	cc.bind('b'-'a'+1,"comando control e b");
	    int              timeout = -1;
	int              randomize = -1;
	int              opt_index = 0;
	int              vt = 0;
	int              backup = 0;
	int              preserve = 0;

	struct ida_image *fimg    = NULL;
	struct ida_image *simg    = NULL;
	struct ida_image *img     = NULL;
	float            scale    = 1;
	float            newscale = 1;

//	int              c
	int editable = 0, once = 0;
	int              i, arg, key;

	char             *line, *info, *desc;
    	char             linebuffer[128];

	setlocale(LC_ALL,"");
	char c;


	setlocale(LC_ALL,"");
    	for (;;) {
	    c = getopt_long(argc, argv, "u1evahPqVbpr:t:m:d:g:s:f:l:T:E:DNh",
			fim_options, &opt_index);
	if (c == -1)
	    break;
	switch (c) {
	case 0:
	    // long option, nothing to do
	    break;
	case '1':
//	    once = 1;
	    break;
	case 'a':
	    cc.setVariable("autotop",1);
//	    autoup   = 1;
//	    autodown = 1;
	    break;
	case 'q':
//	    statusline = 0;
	    break;
	case 'v':
//	    statusline = 1;
	    cc.setVariable("verbose",1);
	    break;
	case 'P':
//	    textreading = 1;
	    cc.setVariable("autowidth",1);
	    break;
	case 'g':
//	    fbgamma = atof(optarg);
	    break;
	case 'r':
//	    pcd_res = atoi(optarg);
	    break;
	case 's':
//	    steps = atoi(optarg);
	    break;
	case 't':
//	    timeout = atoi(optarg);
	    break;
	case 'u':
//	    randomize = 1;
	    break;
	case 'd':
//	    fbdev = optarg;
	    break;
	case 'm':
//	    fbmode = optarg;
	    break;
	case 'f':
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
	    break;
	case 'l':
//	    flist_add_list(optarg);
	    break;
	case 'T':
	    vt = atoi(optarg);
	    break;
	case 'V':
	    version();
	    return 0;
	    break;
	case 'E':
	    cc.scripts.push_back(optarg);
	    /*
	     *
	     * cc.execute(optarg,0);
	     *
	     */
	    break;
	case 'D':
	    cc.setNoFrameBuffer();	// no framebuffer (debug) mode
	    break;
	case 'N':
		cc.setVariable("no_rc_file",1);
	    break;
	default:
	case 'h':
//	    usage(argv[0]);
	    printf("The help will be here soon!\n");
	    printf("Please read the documentation distributed with the program first!\n");
	    exit(0);
//	    cc.exit(1);
	;
	}
    }
	for (i = optind; i < argc; i++)
	{
		//flist_add(argv[i]);
		cc.push(argv[i]);
	}
	lexer=new yyFlexLexer;	//used by YYLEX
	fim::CommandConsole *ccp;
//	main_hack( );
//

#ifndef FIM_NOFB
	if(!cc.noFrameBuffer())
	{
//    std::cerr << "0\n";
	//initialization of the framebuffer text
		fb_text_init1(fontname);
//    std::cerr << "1\n";
	//initialization of the framebuffer text
		fd = fb_init(fbdev, fbmode, vt);
//    std::cerr << "2\n";
	//setting signals to handle in the right ways signals
		fb_catch_exit_signals();
	//???
//		std::cout << "..1..\n";
		fb_switch_init();
		/*
		 *	SEMBRA CHE COMMENTARE LA SEGUENTE RIGA NON ABBIA EFFETTO!
		 */
		signal(SIGTSTP,SIG_IGN);
		//set text color to white ?
		fb_text_init2();
//    		std::cout << "..2..\n";
  

		switch (fb_var.bits_per_pixel) {
	case 8:
		svga_dither_palette(8, 8, 4);
		dither = TRUE;
		init_dither(8, 8, 4, 2);
		break;
	case 15:
    	case 16:
        	if (fb_fix.visual == FB_VISUAL_DIRECTCOLOR)
        	    linear_palette(5);
		if (fb_var.green.length == 5) {
		    lut_init(15);
		} else {
		    lut_init(16);
		}
		break;
	case 24:
        	if (fb_fix.visual == FB_VISUAL_DIRECTCOLOR)
      	      linear_palette(8);
		break;
	case 32:
        	if (fb_fix.visual == FB_VISUAL_DIRECTCOLOR)
          	  linear_palette(8);
		lut_init(24);
		break;
	default:
		fprintf(stderr, "Oops: %i bit/pixel ???\n",
			fb_var.bits_per_pixel);
		exit(1);
    	}
    	if (fb_fix.visual == FB_VISUAL_DIRECTCOLOR ||
		fb_var.bits_per_pixel == 8)
	{
		if (-1 == ioctl(fd,FBIOPUTCMAP,&cmap)) {
	    		perror("ioctl FBIOPUTCMAP");
		    exit(1);
		}
	}
	}
	// svga main loop 
//	std::cerr << "terminal is NOT YET in raw mode now.\n";
	tty_raw();
//    	std::cerr << "terminal is in raw mode now.\n";
	desc = NULL;
	info = NULL;
	fd_set          set;
	struct timeval  limit;
	FD_SET(0, &set);
	limit.tv_sec = timeout;
	limit.tv_usec = 0;
	if (FD_ISSET(0,&set))
	{
		/* stdin, i.e. keyboard */
// 	   rl::redisplay();
// 	   rl::redisplay();
//  	  	rl::redisplay();
/*    		char *rl = readline(":");
  		  if(rl && *rl && *rl!='\n')add_history(rl);
		    if(rl)free(rl);*/
//		cc.execute();
//	  fb_setcolor(100);
    	}
#endif
	cc.init();
	cc.executionCycle();
	return 0;
}


