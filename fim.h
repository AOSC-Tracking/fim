#ifndef FBVI_H
#define FBVI_H
/*
 *	This is the main fim program file.
 *	Created 20061225.
 * */
#include <cstdlib>
#include <cstdio>

#include <cstring>
#include <iostream>
#include <algorithm>
#include <map>
#include <stack>
//#include <fim::string>
#include <vector>
#include <utility>
#include <list>
#include <map>
#include <set>
#include <cassert>

#include <readline/readline.h>
#include <readline/history.h>
#include <regex.h>
//
#include <termios.h>
#include <fcntl.h>
#include "common.h"
//#include <unistd.h>
//
namespace fim{
	void tty_restore();
	void tty_raw(void);
	void cleanup_and_exit(int code);
};


void status_screen(const char *desc, char *info);
namespace fim{
class Arg;
class Browser;
class CommandConsole;
class Command;
class Image;
class Var;
class fim_stream;
//	extern static fim_stream cout;
};

namespace rl{
/*typedef struct
{
	char *name;		// User printable name of the function.
	rl_icpfunc_t *func;	// Function to call to do the job.
	char *doc;		// Documentation for this function.
}COMMAND;
*/

char ** fim_completion (const char *text, int start,int end);
void completion_display_matches_hook(char **matches,int num,int max);
void redisplay();
int redisplay_hook();
void initialize_readline ();
};
#include "extern.h"
#include "string.h"
#include "Command.h"
#include "Arg.h"
#include "Var.h"
#include "Image.h"
#include "Browser.h"
#include "CommandConsole.h"
#include "fim_stream.h"
//#include "src/fbtools.h"


char * dupstr (const char* s);
namespace fim
{
	static fim_stream cout;

class Browser;
class CommandConsole;

std::ostream& operator<<(std::ostream &os,const string& s);

};
void status(const char *desc, const char *info);
void fb_status_screen(const char *msg, int noDraw);

#define FIM_MAX_SCRIPT_FILE (4096*4)

#endif
