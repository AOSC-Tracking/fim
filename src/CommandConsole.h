/* $Id$ */
/*
 CommandConsole.h : Fim console dispatcher header file

 (c) 2007 Michele Martone

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

#ifndef CC_FBVI_H
#define CC_FBVI_H
#include "fim.h"
//#include <stdio.h>
#include <sys/resource.h>


namespace fim
{
class CommandConsole
{
#ifdef FIM_WINDOWS
	fim::Window * window;
#endif
	/*
	 * the registered command methods and objects
	 */
	std::vector<Command*> commands;			//command->method

	/*
	 * the aliases to actions (compounds of commands)
	 */
	std::map<fim::string,fim::string> aliases;	//alias->commands
	
	/*
	 * bindings of key codes to actions (compounds of commands)
	 */
	std::map<int,fim::string> bindings;		//code->commands

	/*
	 * mapping of key name to key code
	 */
	std::map<fim::string,int > key_bindings;	//symbol->code
	std::map<int, fim::string> inverse_key_bindings;//code->symbol

	/*
	 * the image browser logic
	 */
	Browser browser;

	/*
	 * the identifier->variable binding
	 */
	std::map<const fim::string,Var> variables;	//id->var

	/*
	 * the buffer of marked files
	 */
	std::set<fim::string> marked_files;		//filenames

	/*
	 * flags
	 */
	int 	ic;					//in console if 1. not if 0. willing to exit from console mode if -1
	int	cycles;			//FIX ME
	int	exitBinding;				//The key bound to exit. If 0, the special "Any" key.

#ifdef FIM_AUTOCMDS
	/*
	 * the mapping structure for autocommands (<event,pattern,action>)
	 */
	typedef std::map<fim::string,std::vector<fim::string> >  autocmds_p_t;	//pattern - commands
	typedef std::map<fim::string,autocmds_p_t >  autocmds_t;		//autocommand - pattern - commands
	autocmds_t autocmds;
#endif
	
	/*
	 * the last executed action (being a command line or key bounded command issued)
	 */
	fim::string last_action;
	
#ifdef FIM_RECORDING
	bool recordMode;//WORKON...
	typedef std::pair<fim::string,int> recorded_action_t;
	typedef std::vector<recorded_action_t > recorded_actions_t;
	recorded_actions_t recorded_actions;

	void clearRecordBuffer(){}
	bool dont_record_last_action;
	fim::string memorize_last(const fim::string &cmd);
	fim::string repeat_last(const std::vector<fim::string> &args);
	fim::string dump_record_buffer(const std::vector<fim::string> &args);
	fim::string execute_record_buffer(const std::vector<fim::string> &args);
	fim::string start_recording(const std::vector<fim::string> &args);
	fim::string stop_recording(const std::vector<fim::string> &args);
	fim::string sanitize_action(const fim::string &cmd);

	void record_action(const fim::string &cmd);
#endif

	int fim_stdin;	// the standard input file descriptor
	char prompt[2];

#ifndef FIM_NOSCRIPTING
	std::vector<fim::string> scripts;		//scripts to execute : FIX ME PRIVATE
#endif

	void markCurrentFile();
	public:

	fim::string execute(fim::string cmd, std::vector<fim::string> args);

	const char*get_prompt(){return prompt;}

	CommandConsole();

	fim::string markCurrentFile(const std::vector<fim::string>& args){markCurrentFile();return "";}
	bool display();
	bool redisplay();
	char * command_generator (const char *text,int state);
	void executionCycle();
	void init();
	int  inConsole()const{return ic==1;};
	~CommandConsole();
	float getFloatVariable(const fim::string &varname);
	fim::string getStringVariable(const fim::string &varname);
	int  getVariableType(const fim::string &varname);
	int  getIntVariable(const fim::string & varname);
	int  printVariable(const fim::string & varname);
	int  setVariable(const fim::string& varname,int value);
	float setVariable(const fim::string& varname,float value);
	int setVariable(const fim::string& varname,const char*value);
	bool push(const fim::string nf);
	int executeStdFileDescriptor(FILE *fd);
	fim::string readStdFileDescriptor(FILE* fd);
#ifndef FIM_NOSCRIPTING
	bool push_script(const fim::string ns);
	fim::string executeFile(const std::vector<fim::string> &args);
#endif
	private:
	fim::string echo(const std::vector<fim::string> &args);
	fim::string help(const std::vector<fim::string> &args);
	fim::string quit(const std::vector<fim::string> &args);
	fim::string foo (const std::vector<fim::string> &args);
	fim::string do_return(const std::vector<fim::string> &args);
	fim::string status(const std::vector<fim::string> &args);
	int  executeFile(const char *s);
	void execute(const char *ss, int add_history_, int suppress_output_);

	int  toggleStatusLine();
	int  addCommand(Command *c);
	Command* findCommand(fim::string cmd);
	fim::string alias(std::vector<Arg> args);
	fim::string alias(const fim::string& a,const fim::string& c);
	fim::string aliasRecall(fim::string cmd);
	fim::string system(const std::vector<fim::string>& args);
	fim::string cd(const std::vector<fim::string>& args);
	fim::string pwd(const std::vector<fim::string>& args);
	fim::string sys_popen(const std::vector<fim::string>& args);
	fim::string set_interactive_mode(const std::vector<fim::string>& args);
	fim::string set_in_console(const std::vector<fim::string>& args);
	fim::string autocmd(const std::vector<fim::string>& args);
	fim::string autocmd_del(const fim::string &event,const fim::string &pat){return "";}
	fim::string autocmd_add(const fim::string &event,const fim::string &pat,const fim::string &cmd);
	fim::string autocmds_list();
	typedef std::pair<fim::string,fim::string> autocmds_frame_t;
	typedef std::set<autocmds_frame_t> autocmds_stack_t;
	autocmds_stack_t autocmds_stack;
	std::vector<fim::string> autocmds_sub_list(const fim::string &event);
	fim::string bind(const std::vector<fim::string>& args);
	fim::string getAliasesList();
	fim::string dummy(std::vector<Arg> args);
	fim::string variables_list(const std::vector<fim::string>& args){return get_variables_list();}
	fim::string unalias(const std::vector<fim::string>& args);
	char ** tokenize_(const char *s);
	void executeBinding(const int c);
	fim::string getBoundAction(const int c);
//	void execute(fim::string cmd);
	fim::string eval(const std::vector<fim::string> &args);
	void exit(int i);
	fim::string unbind(int c);
	fim::string bind(int c,fim::string binding);
	fim::string unbind(const fim::string& key);
	fim::string unbind(const std::vector<fim::string>& args);
	fim::string getBindingsList();
	fim::string dump_key_codes(const std::vector<fim::string>& args);
	fim::string clear(const std::vector<fim::string>& args){status_screen(NULL,NULL);return "";}
	void quit(int i=0);
	public:
	int  drawOutput();
	bool regexp_match(const char*s, const char*r)const;
#ifdef FIM_AUTOCMDS
	fim::string autocmd_exec(const fim::string &event,const fim::string &fname);
	fim::string pre_autocmd_add(const fim::string &cmd);
#endif
	int catchLoopBreakingCommand(int seconds=0);

	private:
	int catchInteractiveCommand(int seconds=0);
#ifdef FIM_AUTOCMDS
	fim::string autocmd_exec(const fim::string &event,const fim::string &pat,const fim::string &fname);
	void autocmd_push_stack(const autocmds_frame_t& frame);
	void autocmd_pop_stack(const autocmds_frame_t& frame);
	int  autocmd_in_stack(const autocmds_frame_t& frame);
#endif
	fim::string current()const{ return browser.current();}

	fim::string get_alias_info(const fim::string aname);
	const Window & current_window()const;
	public:

	fim::string get_variables_list()const;
	fim::string get_aliases_list()const;
	fim::string get_commands_list()const;

	void printHelpMessage(char *pn="fim");
	void appendPostInitCommand(const char* c);
	void appendPostExecutionCommand(const fim::string &c);

	#ifdef FIM_WINDOWS
	Viewport& current_viewport()const;
	
	void dumpDefaultFimrc()const;
	#endif
	private:

	fim::string postInitCommand;
	fim::string postExecutionCommand;

	int show_must_go_on;
};
}

#endif
