/* $Id$ */
/*
 readline.cpp : Code dealing with the GNU readline library.

 (c) 2008 Michele Martone

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

#include <iostream>
#include "readline.h"
#include "CommandConsole.h"

#define min(x,y) ((x)<(y)?(x):(y))
#define max(x,y) ((x)>(y)?(x):(y))

/*
 * This file is severely messed up :).
 * */

namespace fim
{
	extern CommandConsole cc;
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
static char ** fim_completion (const char *text, int start,int end)
{
	//FIX ME
	char **matches = (char **)NULL;

	if(start==end && end<1)
	{
#if 0
		char **__s,*_s;
		_s=dupstr("");
		if(! _s)return NULL;
		__s=(char**)calloc(1,sizeof(char*));
		if(!__s)return NULL;__s[0]=_s;
		//we print all of the commands, with no completion, though.
#endif
		cc.print_commands();
		rl_attempted_completion_over = 1;
		/* this could be set only here :) */
		return NULL;
	}
	

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
static void completion_display_matches_hook(char **matches,int num,int max)
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
		w=min(strlen(matches[i])+1,(size_t)f);
		if(f>0){
		strncpy(buffer+l,matches[i],w);
		w=strlen(buffer+l);l+=w;f-=w;}
		if(f>0){strcpy(buffer+l," ");--f;++l;}
		buffer[l]='\0';
//		strcpy(buffer+strlen(buffer),matches[i]);
//		strcpy(buffer+strlen(buffer)," ");
	}
	//      status_screen((unsigned char*)buffer, NULL);
	
//	fb_status_screen(buffer);


//	std::cout << buffer << "\n" ;
 //     status((unsigned char*)"here shall be autocompletions", NULL);
}

static void redisplay_no_fb()
{
	printf("%s",rl_line_buffer);
}

static void redisplay()
{	
	/*
	 * DANGER
	 *  buffer overflow awaiting
	 */
	//static int c=100;
//	fb_setcolor(c=~c);//sleep(1);
	cc.set_status_bar(( char*)rl_line_buffer,NULL);
//	fprintf(stderr,"::%s\n",rl_line_buffer);
//	fprintf(stdout,"::%s\n",rl_line_buffer);
}

/*
 * ?!
 * */
static int redisplay_hook_no_fb()
{
	redisplay_no_fb();
	return 0;
}

static int redisplay_hook()
{
	redisplay();
	return 0;
}

/*
 * ?!
 * */
static int fim_rl_end(int a,int b)
{
	rl_point=rl_end;
	return 0;
}

/*
 * ?!
 * */
static int fim_set_command_line_text(const char*s)
{
	rl_replace_line(s,0);
	return 0;
}

/*
 *	initial setup to set the readline library working
 */
void initialize_readline (int with_no_framebuffer)
{
	//FIX ME
	/* Allow conditional parsing of the ~/.inputrc file. */
	rl_readline_name = "fim";	//??
	/* Tell the completer that we want a crack first. */
	rl_attempted_completion_function = fim_completion;
	rl_completion_display_matches_hook=completion_display_matches_hook;

	if(with_no_framebuffer==0)
	{
		rl_catch_signals=0;
		rl_catch_sigwinch=0;
		rl_redisplay_function=redisplay;
	        rl_event_hook=redisplay_hook;
	        rl_pre_input_hook=redisplay_hook;
	}
	//rl_completion_entry_function=NULL;
	/*
	 * to do:
	 * see rl_filename_quoting_function ..
	 * */
	//rl_inhibit_completion=1;	//if set, TABs are read as normal characters
	rl_filename_quoting_desired=1;
	rl_filename_quote_characters="\"";
	//rl_reset_terminal("linux");
	//rl_reset_terminal("vt100");
	//rl_bind_key(0x09,fim_rl_end);
	//rl_bind_key(0x7F,fim_rl_end);
	//rl_bind_key(-1,fim_rl_end);
	//rl_bind_key('~',fim_rl_end); // ..
	//rl_bind_key('\t',rl_insert);
	//rl_bind_keyseq("g",fim_rl_end);
	//rl_set_prompt("$");

/*	rl_voidfunc_t *rl_redisplay_function=redisplay;
	rl_hook_func_t *rl_event_hook=redisplay_hook;
	rl_hook_func_t *rl_pre_input_hook=redisplay_hook;*/
	//std::cout << "readline initialized\n";
}

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
	return fim::cc.command_generator(text,state);

		
//	if (!state) { list_index = 0; len = strlen (text); }

        /* Return the next name which partially matches from the
	 * command list.
	 */

//	while (name = commands[list_index].name)
//	{ list_index++; if (strncmp (name, text, len) == 0) return (dupstr(name)); }
	/* If no names matched, then return NULL. */
//	return ((char *)NULL);
}


}


