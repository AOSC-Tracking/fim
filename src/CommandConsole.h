/* $Id$ */
/*
 CommandConsole.h : Fim console dispatcher header file

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

#ifndef CC_FBVI_H
#define CC_FBVI_H
#include "fim.h"
//#include <stdio.h>
#include <sys/resource.h>
namespace fim
{
class CommandConsole
{
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
	int	cycles,isinscript;			//FIX ME
	int	nofb;					//FIX ME
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

	public:
	const char*get_prompt(){return prompt;}

	std::vector<fim::string> scripts;		//scripts to execute : FIX ME PRIVATE
	
	int noFrameBuffer(){return nofb!=0;}
	void setNoFrameBuffer(){nofb=1;}
	CommandConsole();
	void markCurrentFile();
	fim::string markCurrentFile(const std::vector<fim::string>& args){markCurrentFile();return "";}
	void display();
	char * command_generator (const char *text,int state);
	void executionCycle();
	void init();
	int  inConsole()const{return ic==1;};
	fim::string execute(fim::string cmd, std::vector<fim::string> args);
	~CommandConsole();
	float getFloatVariable(const fim::string &varname);
	fim::string getStringVariable(const fim::string &varname);
	int getVariableType(const fim::string &varname);
	int  getIntVariable(const fim::string & varname);
	int  printVariable(const fim::string & varname);
	int  setVariable(const fim::string& varname,int value);
	float setVariable(const fim::string& varname,float value);
	int setVariable(const fim::string& varname,const char*value);
	bool push(const fim::string nf){return browser.push(nf);}
	int executeStdFileDescriptor(FILE *fd);
#ifndef FIM_NOSCRIPTING
	int  executeFile(const char *s);
	fim::string executeFile(const std::vector<fim::string> &args);
	int executeFileDescriptor(int fd);
#endif
	fim::string echo(const std::vector<fim::string> &args);
	fim::string help(const std::vector<fim::string> &args);
	fim::string quit(const std::vector<fim::string> &args);
	fim::string foo (const std::vector<fim::string> &args);
	fim::string status(const std::vector<fim::string> &args);
	void execute(const char *s, int add_history_);
	private:
	int  toggleStatusLine();
	int  addCommand(Command *c);
	Command* findCommand(fim::string cmd);
	fim::string alias(std::vector<Arg> args);
	fim::string alias(const fim::string& a,const fim::string& c);
	fim::string aliasRecall(fim::string cmd);
	fim::string system(const std::vector<fim::string>& args);
	fim::string sys_popen(const std::vector<fim::string>& args);
	fim::string set_interactive_mode(const std::vector<fim::string>& args);
	fim::string autocmd(const std::vector<fim::string>& args);
	fim::string autocmd_del(const fim::string &event,const fim::string &pat){return "";}
	fim::string autocmd_add(const fim::string &event,const fim::string &pat,const fim::string &cmd);
	fim::string autocmds_list();
	typedef std::pair<fim::string,fim::string> autocmds_frame_t;
	typedef std::set<autocmds_frame_t> autocmds_stack_t;
	autocmds_stack_t autocmds_stack;
	std::vector<fim::string> autocmds_sub_list(const fim::string &event);
	fim::string bind(const std::vector<fim::string>& args);
#if 0
	fim::string bind(std::vector<Arg> args);
#endif
	fim::string getAliasesList();
	fim::string dummy(std::vector<Arg> args);
	fim::string variables_list(const std::vector<fim::string>& args){return get_variables_list();}
	fim::string unalias(const std::vector<fim::string>& args);
	char ** tokenize_(const char *s);
	void executeBinding(const int c);
	fim::string getBoundAction(const int c);
//	void execute(fim::string cmd);
	void exit(int i);
	fim::string unbind(int c);
	fim::string bind(int c,fim::string binding);
	fim::string unbind(const fim::string& key);
	fim::string unbind(const std::vector<fim::string>& args);
	fim::string getBindingsList();
	fim::string dump_key_codes(const std::vector<fim::string>& args);
	fim::string clear(const std::vector<fim::string>& args){status_screen(NULL,NULL);return "";}
	public:
	void quit(int i=0);
	int  drawOutput();
	int  isInScript(){return isinscript;}
	bool regexp_match(const char*s, const char*r)const;
#ifdef FIM_AUTOCMDS
	fim::string autocmd_exec(const fim::string &event,const fim::string &fname);
#endif
	int current_image (){return browser.current_image ();}
	int current_images(){return browser.current_images();}
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
	public:
	fim::string get_variables_list();
	fim::string get_aliases_list();
	fim::string get_commands_list();
	void printHelpMessage(char *pn="fim");
	void appendPostInitCommand(const fim::string &c)
	{
		postInitCommand+=c;
	}
	void appendPostExecutionCommand(const fim::string &c)
	{
		postExecutionCommand+=c;
	}
	private:
	fim::string postInitCommand;
	fim::string postExecutionCommand;
	int show_must_go_on;
};
}

#endif
