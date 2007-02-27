#ifndef CC_FBVI_H
#define CC_FBVI_H
#include "fim.h"
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
	bool	ic;					//in console
	int	cycles,isinscript;			//FIX ME
	int	nofb;					//FIX ME


	/*
	 * the mapping structure for autocommands (<event,pattern,action>)
	 */
	typedef std::map<fim::string,std::vector<fim::string> >  autocmds_p_t;	//pattern - commands
	typedef std::map<fim::string,autocmds_p_t >  autocmds_t;		//autocommand - pattern - commands
	autocmds_t autocmds;
	
	/*
	 * the last executed action (being a command line or key bounded command issued)
	 */
	fim::string last_action;
	
	bool recordMode;//WORKON...
	typedef std::pair<fim::string,int> recorded_action_t;
	typedef std::vector<recorded_action_t > recorded_actions_t;
	recorded_actions_t recorded_actions;

	void clearRecordBuffer(){}
	bool dont_record_last_action;
	fim::string memorize_last(const fim::string &cmd)
	{
		//WARNING : DANGER
		if(dont_record_last_action==false)last_action=cmd;
		dont_record_last_action=false;	//from now on we can memorize again
		return "";
	}

	fim::string repeat_last(const std::vector<fim::string> &args)
	{
		/*
		 * WARNING : there is an intricacy concerning the semantics of this command :
		 * - This command should NOT be registered as last_command, nor any alias 
		 *   triggering it. But this solution would require heavy parsing and very
		 *   complicated machinery and information propagation... 
		 * - A solution would be confining the repeat_last only to interactive commands,
		 *   but this would be a lot sorrowful too, and requires the non-registration 
		 *   of the 'repeat_last;' issuing..
		 * - So, since the recording is made AFTER the command was executed, we set
		 *   a dont_record_last_action flag after each detection of repeat_last, so we do not 
		 *   record the containing string.
		 */
		execute(last_action.c_str(),0);
		dont_record_last_action=true;	//the issuing action will not be recorded
		return "";
	}

	fim::string dump_record_buffer(const std::vector<fim::string> &args)
	{
		fim::string res;
		for(unsigned int i=0;i<recorded_actions.size();++i)
			res+=recorded_actions[i].first;
		return res;
	}
	
	fim::string execute_record_buffer(const std::vector<fim::string> &args)
	{
		execute(dump_record_buffer(args).c_str(),0);
		return "";
	}
	
	fim::string start_recording(const std::vector<fim::string> &args)
	{
		recorded_actions.clear();
		recordMode=true;
		return "";
	}

	fim::string  stop_recording(const std::vector<fim::string> &args)
	{
		/*
		 * since the last recorded action was stop_recording, we pop out the last command
		 */
		if(recorded_actions.size()>0)recorded_actions.pop_back();
		recordMode=false;
		return "";
	}

	fim::string sanitize_action(const fim::string &cmd)
	{
		/*
		 * the purpose of this method is to sanitize the action token
		 * in order to gain a dumpable and self standing action
		 */
		if(cmd.c_str()[strlen(cmd.c_str())-1]!=';')
			return cmd+fim::string(";");
		return cmd;
	}

	void record_action(const fim::string &cmd)
	{
		//FIX ME
		//recorded_actions.push_back(recorded_action_t(cmd,0));
		recorded_actions.push_back(recorded_action_t(sanitize_action(cmd),0));
	}

	public:

	std::vector<fim::string> scripts;		//scripts to execute : FIX ME PRIVATE
	
	int noFrameBuffer(){return nofb!=0;}
	void setNoFrameBuffer(){nofb=1;}
	CommandConsole();
	void markCurrentFile(){if(browser.current()!="")marked_files.insert(browser.current());}
	fim::string markCurrentFile(const std::vector<fim::string>& args){markCurrentFile();return "";}
	void display();
	char * command_generator (const char *text,int state);
	void executionCycle();
	void init();
	int  inConsole()const{return ic;};
	fim::string execute(fim::string cmd, std::vector<fim::string> args);
	~CommandConsole();
	float getFloatVariable(const fim::string &varname);
	int getVariableType(const fim::string &varname);
	int  getIntVariable(const fim::string & varname);
	int  setVariable(const fim::string& varname,int value);
	float setVariable(const fim::string& varname,float value);
	bool push(const fim::string nf){return browser.push(nf);}
	int  executeFile(const char *s);
	fim::string echo(const std::vector<fim::string> &args);
	fim::string executeFile(const std::vector<fim::string> &args);
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
	fim::string autocmd(const std::vector<fim::string>& args);
	fim::string autocmd_del(const fim::string &event,const fim::string &pat){return "";}
	fim::string autocmd_add(const fim::string &event,const fim::string &pat,const fim::string &cmd);
	fim::string autocmds_list();
	typedef std::pair<fim::string,fim::string> autocmds_frame_t;
	typedef std::set<autocmds_frame_t> autocmds_stack_t;
	autocmds_stack_t autocmds_stack;
	std::vector<fim::string> autocmds_sub_list(const fim::string &event);
	fim::string bind(const std::vector<fim::string>& args);
	fim::string bind(std::vector<Arg> args);
	fim::string displayAliases();
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
	fim::string unbind(const std::vector<fim::string>& args);
	fim::string dump_key_codes(const std::vector<fim::string>& args);
	public:
	void quit();
	int  drawOutput();
	int  isInScript(){return isinscript;}
	fim::string autocmd_exec(const fim::string &event,const fim::string &fname);
	int current_image (){return browser.current_image ();}
	int current_images(){return browser.current_images();}
	int catchInteractiveCommand();
	private:
	fim::string autocmd_exec(const fim::string &event,const fim::string &pat,const fim::string &fname);
	void autocmd_push_stack(const autocmds_frame_t& frame);
	void autocmd_pop_stack(const autocmds_frame_t& frame);
	int  autocmd_in_stack(const autocmds_frame_t& frame);
	fim::string current()const{ return browser.current();}
	bool regexp_match(const char*s, const char*r)const;
	fim::string get_variables_list();
};
}

#endif
