/* $LastChangedDate: 2011-07-02 10:01:19 +0200 (Sat, 02 Jul 2011) $ */
/*
 CommandConsole.h : Fim console dispatcher header file

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

#ifndef FIM_COMMANDCONSOLE_H
#define FIM_COMMANDCONSOLE_H
#include "fim.h"
#include "DummyDisplayDevice.h"

#define FIM_WANT_RAW_KEYS_BINDING 1

namespace fim
{
class CommandConsole
{
	friend class FbiStuff;

	private:
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
#ifndef FIM_KEEP_BROKEN_CONSOLE
	public:
	MiniConsole mc_;
#endif
#endif
	FontServer fontserver_;

	fim::string postInitCommand_;
	fim::string preConfigCommand_;
	fim::string postExecutionCommand_;

	int show_must_go_on_;
	int return_code_;	/* new, to support the 'return' command */
	bool mangle_tcattr_;
	public:

	struct termios  saved_attributes_;
	int             saved_fl_;

	/*
	 * the image browser_ logic
	 */
	Browser browser_;
	private:

#ifdef FIM_WINDOWS
	public:// 20110617 this is terrible, I know
	fim::Window * window_;
	private:
#endif
	/*
	 * the registered command methods and objects
	 */
	std::vector<Command*> commands_;			//command->method

	/*
	 * the aliases to actions (compounds of commands)
	 */
	typedef std::map<fim::string,std::pair<fim::string,fim::string> > aliases_t;	//alias->[commands,description]
	//typedef std::map<fim::string,fim::string> aliases_t;	//alias->commands
	aliases_t aliases_;	//alias->commands
	
	/*
	 * bindings of key codes to actions (compounds of commands)
	 */
	typedef std::map<fim_key_t,fim::string> bindings_t;		//code->commands
	bindings_t bindings_;		//code->commands

	/*
	 * mapping of key name to key code
	 */
	sym_keys_t	sym_keys_;	//symbol->code

	typedef std::map<fim_key_t, fim::string> key_syms_t;//code->symbol
	key_syms_t key_syms_;//code->symbol

	private:

	fim_err_t save_history();
	fim_err_t load_history();

	/*
	 * the identifier->variable binding
	 */
	typedef std::map<const fim::string,Var> variables_t;	//id->var
	variables_t variables_;	//id->var

#if FIM_WANT_FILENAME_MARK_AND_DUMP
	/*
	 * the buffer of marked files
	 */
	typedef std::set<fim::string> marked_files_t;	//
	marked_files_t marked_files_;		//filenames
#endif

	/*
	 * flags
	 */
#ifdef FIM_USE_READLINE
	/* no readline ? no console ! */
	fim_status_t 	ic_;				//in console if 1. not if 0. willing to exit from console mode if -1
#endif
	fim_cycles_t cycles_;					//fim execution cycles_ counter (quite useless)
	fim_key_t exitBinding_;				//The key bound to exit. If 0, the special "Any" key.

#ifdef FIM_AUTOCMDS
	/*
	 * the mapping structure for autocommands (<event,pattern,action>)
	 */
	typedef std::map<fim::string,args_t >  autocmds_p_t;	//pattern - commands
	typedef std::map<fim::string,autocmds_p_t >  autocmds_t;		//autocommand - pattern - commands
	autocmds_t autocmds_;
#endif
	
	/*
	 * the last executed action (being a command line or key bounded command issued)
	 */
	fim::string last_action_;
	
#ifdef FIM_RECORDING
	bool recordMode_;
	typedef std::pair<fim::string,fim_tms_t > recorded_action_t;
	typedef std::vector<recorded_action_t > recorded_actions_t;
	recorded_actions_t recorded_actions_;

	bool dont_record_last_action_;
	fim::string memorize_last(const fim::string &cmd);
	fim::string repeat_last(const args_t &args);
	fim::string dump_record_buffer(const args_t &args);
	fim::string do_dump_record_buffer(const args_t &args)const;
	fim::string execute_record_buffer(const args_t &args);
	fim::string start_recording();
	fim::string fcmd_recording(const args_t &args);
	fim::string stop_recording();
	fim::string sanitize_action(const fim::string &cmd)const;

	void record_action(const fim::string &cmd);
#endif

	public:
	int fim_stdin_;	// the standard input file descriptor
	private:
	fim_char_t prompt_[2];

#ifndef FIM_WANT_NOSCRIPTING
	args_t scripts_;		//scripts to execute : FIX ME PRIVATE
#endif

#if FIM_WANT_FILENAME_MARK_AND_DUMP
	public:
	void markCurrentFile();
	void unmarkCurrentFile();
	private:
#endif
#ifdef FIM_WITH_AALIB
	AADevice * aad_;
#endif
	public:
	DummyDisplayDevice dummydisplaydevice_;
	DisplayDevice *displaydevice_;

	fim::string execute(fim::string cmd, args_t args);

	const char*get_prompt()const{return prompt_;}

	CommandConsole();
	private:
	CommandConsole& operator= (const CommandConsole&cc){return *this;/* a nilpotent assignation */}
	public:
	bool display();
	bool redisplay();
	char * command_generator (const char *text,int state,int mask)const;
	fim_err_t executionCycle();
	fim_err_t init(fim::string device);
	int  inConsole()const;
	~CommandConsole();
	float getFloatVariable(const fim::string &varname)const;
	fim::string getStringVariable(const fim::string &varname)const;
	fim_var_t getVariableType(const fim::string &varname)const;
	int  getIntVariable(const fim::string & varname)const;
	Var  getVariable(const fim::string & varname)const;
	fim_err_t printVariable(const fim::string & varname)const;
	int  setVariable(const fim::string& varname,int value);
	float setVariable(const fim::string& varname,float value);
	int setVariable(const fim::string& varname,const char*value);
	Var setVariable(const fim::string varname,const Var&value);//NEW
	bool push(const fim::string nf);
	fim_err_t executeStdFileDescriptor(FILE *fd);
	fim::string readStdFileDescriptor(FILE* fd);
#ifndef FIM_WANT_NOSCRIPTING
	bool push_scriptfile(const fim::string ns);
	bool with_scriptfile()const;
	fim::string fcmd_executeFile(const args_t &args);
#endif
	private:
	fim::string fcmd_echo(const args_t &args);
	fim::string do_echo(const args_t &args)const;
	//	fim::string get_expr_type(const args_t &args);
	fim::string fcmd_help(const args_t &args);
	fim::string fcmd_quit(const args_t &args);
	fim::string fcmd__stdout(const args_t &args);
	/* naming this stdout raises problems on some systems 
	   e.g.: 
# uname -a
Darwin hostname 7.9.0 Darwin Kernel Version 7.9.0: Wed Mar 30 20:11:17 PST 2005; root:xnu/xnu-517.12.7.obj~1/RELEASE
# gcc -v
Reading specs from /usr/libexec/gcc/darwin/ppc/3.3/specs
Thread model: posix
gcc version 3.3 20030304 (Apple Computer, Inc. build 1495)
*/
	fim::string fcmd_foo (const args_t &args);
	fim::string fcmd_status(const args_t &args);
	fim_err_t executeFile(const char *s);
	fim_err_t execute_internal(const char *ss, fim_xflags_t xflags);

	fim_err_t toggleStatusLine();
	fim_err_t addCommand(Command *c);
	Command* findCommand(fim::string cmd)const;
	int findCommandIdx(fim::string cmd)const;
	fim::string fcmd_alias(std::vector<Arg> args);
	fim::string alias(const fim::string& a,const fim::string& c, const fim::string& d="");
	fim::string aliasRecall(fim::string cmd)const;
	fim::string fcmd_system(const args_t& args);
	fim::string fcmd_cd(const args_t& args);
	fim::string fcmd_pwd(const args_t& args);
	fim::string fcmd_sys_popen(const args_t& args);
#ifdef FIM_PIPE_IMAGE_READ
	fim::string fcmd_pread(const args_t& args);
#endif
	public:// 20110601
	fim_err_t fpush(FILE *tfd);
	private:
	fim::string fcmd_set_interactive_mode(const args_t& args);
	fim::string fcmd_set_in_console(const args_t& args);
#ifdef FIM_AUTOCMDS
	fim::string fcmd_autocmd(const args_t& args);
	fim::string autocmd_del(const fim::string event, const fim::string pattern, const fim::string action);
	fim::string fcmd_autocmd_del(const args_t& args);
	public:// 20110601
	fim::string autocmd_add(const fim::string &event,const fim::string &pat,const fim::string &cmd);
	private:
	fim::string autocmds_list(const fim::string event, const fim::string pattern)const;
#endif
	typedef std::pair<fim::string,fim::string> autocmds_loop_frame_t;
	typedef std::pair<autocmds_loop_frame_t,fim::string> autocmds_frame_t;
	typedef std::vector<autocmds_loop_frame_t > autocmds_stack__t;
	typedef std::vector<autocmds_frame_t > autocmds_stack_t;
	//typedef std::set<autocmds_frame_t> autocmds_stack_t;
	autocmds_stack__t autocmds_loop_stack;
	autocmds_stack_t autocmds_stack;
	fim::string fcmd_bind(const args_t& args);
	fim::string getAliasesList()const;
	fim::string dummy(std::vector<Arg> args);
	fim::string fcmd_variables_list(const args_t& args){return get_variables_list();}
	fim::string fcmd_commands_list(const args_t& args){return get_commands_list();}
	fim::string fcmd_set(const args_t &args);
	fim::string fcmd_unalias(const args_t& args);
	char ** tokenize_(const char *s);
	void executeBinding(const fim_key_t c);
	fim::string getBoundAction(const fim_key_t c)const;
	//	void execute(fim::string cmd);
	fim::string fcmd_eval(const args_t &args);
	void exit(int i)const;// FIXME: exit vs quit
	fim::string unbind(fim_key_t c);
	fim::string bind(fim_key_t c,fim::string binding);
	public:
	fim::string find_key_for_bound_cmd(fim::string binding);
	fim_err_t execDefaultConfiguration();
	private:
	fim::string unbind(const fim::string& key);
	fim::string fcmd_unbind(const args_t& args);
	fim::string getBindingsList()const;
	fim::string fcmd_dump_key_codes(const args_t& args);
	fim::string do_dump_key_codes(const args_t& args)const;
#ifndef FIM_WANT_NO_OUTPUT_CONSOLE
	fim::string fcmd_clear(const args_t& args);
	fim::string scroll_up(const args_t& args);
	fim::string scroll_down(const args_t& args);
#endif
	int quit(int i=FIM_CNS_ERR_QUIT);
	public:
	fim_key_t find_keycode_for_bound_cmd(fim::string binding);

	fim_bool_t drawOutput(const char*s=NULL)const;
	bool regexp_match(const char*s, const char*r)const;
#ifdef FIM_AUTOCMDS
	fim::string autocmd_exec(const fim::string &event,const fim::string &fname);
	fim::string pre_autocmd_add(const fim::string &cmd);
	fim::string pre_autocmd_exec();
#endif
	int catchLoopBreakingCommand(fim_ts_t seconds=0);

	private:
	fim_key_t catchInteractiveCommand(fim_ts_t seconds=0)const;
#ifdef FIM_AUTOCMDS
	fim::string autocmd_exec(const fim::string &event,const fim::string &pat,const fim::string &fname);
	void autocmd_push_stack(const autocmds_loop_frame_t& frame);
	void autocmd_pop_stack(const autocmds_loop_frame_t& frame);
	public:
	void autocmd_trace_stack();
	private:
	fim_bool_t autocmd_in_stack(const autocmds_loop_frame_t& frame)const;
#endif
	fim::string current()const{ return browser_.current();}

	fim::string get_alias_info(const fim::string aname)const;
#ifdef FIM_WINDOWS
	const Window & current_window()const;
#endif
	fim::string get_variables_list()const;
	fim::string get_aliases_list()const;
	fim::string get_commands_list()const;
	public:

	void printHelpMessage(char *pn="fim")const;
	void appendPostInitCommand(const char* c);
	void appendPreConfigCommand(const char* c);
	void appendPostExecutionCommand(const fim::string &c);
	bool appendedPostInitCommand()const;
	bool appendedPreConfigCommand()const;

	Viewport* current_viewport()const;
#ifdef FIM_WINDOWS
#else
	Viewport* viewport_;
#endif
	void dumpDefaultFimrc()const;

	void tty_raw();
	void tty_restore();
	void cleanup();
	
	fim::string print_commands()const;

	void status_screen(const char *desc);
	void set_status_bar(fim::string desc, const char *info);
	void set_status_bar(const char *desc, const char *info);
        bool is_file(fim::string nf)const;
	fim::string fcmd_do_getenv(const args_t& args);
	bool isVariable(const fim::string &varname)const;
	fim::string dump_reference_manual(const args_t& args);
	fim::string get_reference_manual(const args_t& args);
	private:
	fim::string get_commands_reference(FimDocRefMode refmode=DefRefMode)const;
	fim::string get_variables_reference(FimDocRefMode refmode=DefRefMode)const;
	public:
	fim_err_t resize(fim_coo_t w, fim_coo_t h);
	fim_err_t display_reinit(const fim_char_t *rs);
	fim::string fcmd_basename(const args_t& args);
	fim_bool_t key_syms_update();
};
}

#endif
