
#include "fim.h"

extern int yyparse();

namespace fim
{
	int nochars(const char *s)
	{
		if(s==NULL)return 1;
		while(*s && isspace(*s))++s;
		return *s=='\0'?1:0;
	}

	Command* CommandConsole::findCommand(fim::string cmd)
	{
		/*
		 * is cmd a valid internal Fim command ?
		 */
		for(int i=0;i<commands.size();++i) 
			if(commands[i] && commands[i]->cmd==cmd)
				return commands[i];
		return NULL;
	}

	fim::string CommandConsole::bind(int c,const fim::string binding)
	{
		/*
		 * binds keycode c to the action specified in binding

		 * note : the binding translation map is used as a necessary
		 * indirection...
		 */
		if(bindings[c]!="")
		{
			bindings[c]=binding;
			return fim::string("binding ")+fim::string(c)
				+fim::string(" successfully replaced to ")+fim::string(bindings[c])+fim::string("\n");
		}
		else
		{
			bindings[c]=binding;
			return fim::string("binding ")+fim::string(c)
				+fim::string(" successfully added to ")+fim::string(bindings[c])+fim::string("\n");
		}
	}

	fim::string CommandConsole::bind(const std::vector<fim::string>& args)
	{
		/*
		 *	this is the interactive bind command
		 *
		 *	FIX ME
		 */
		const char *kerr="bind : invalid key argument (should be one of : k, C-k, K, <Left..> }\n";
		if(args.size()!=2)return "usage : bind KEY COMMAND\n";
		const char*key=(args[0].c_str());
		if(!key)return kerr;
		int l=strlen(key);
		if(!l)return kerr;
		return bind(key_bindings[args[0]],args[1]);
	}

	fim::string CommandConsole::unbind(const std::vector<fim::string>& args)
	{
		/*
		 * 	unbinds the first specified in args..
		 *	PLEASE FINISH THIS FUNCTION:
		 *	maybe you should made surjective the binding_keys mapping..
		 */
		if(args.size()!=1)return "unbind : specify the key to unbind\n";
		return unbind(key_bindings[args[0]]);
	}

	fim::string CommandConsole::unbind(int c)
	{
		/*
		 * undinds the key with code c
		 */
		if(bindings[c]!="")
		{
			bindings.erase(c);
			return fim::string("unbind ")+fim::string(c)+fim::string(": successfully unbinded.\n");
		}
		else
			return fim::string("unbind ")+fim::string(c)+fim::string(": there were not such binding.\n");
	}

	fim::string CommandConsole::bind(std::vector<Arg> args)
	{
		/*
		 *	FIX ME 
		 */
		/*
		fim::string cmdlist;
		int c,C,tmp;
		if(args.size()<2)
		{
			return "bind <>: cant't add an empty binding!\n";
		}
		if((tmp=strlen(args[0].val.c_str()))<1||tmp>2)
			return "bind <>: bad key specifier!\n";
		if(tmp==2)C= args[0].val.c_str()[0];
		if(tmp==2)c=(args[0].val.c_str()[1]+1)-'a';
		if(tmp==1)c= args[0].val.c_str()[0];
		if((tmp==2)&&(C!='C'))return "bind : wrong format.\n";
		for(int i=1;i<args.size();++i)cmdlist+=args[i].val;
		return bind(c,cmdlist);*/
		return "....";
	}

	fim::string CommandConsole::aliasRecall(fim::string cmd)
	{
		/*
		 * returns the alias command specified by cmd
		 */
		return aliases[cmd];
	}

	fim::string CommandConsole::displayAliases()
	{
		//FIX ME
		fim::string aliases_expanded;
		std::map<fim::string,fim::string>::const_iterator ai;
		for( ai=aliases.begin();ai!=aliases.end();++ai)
		{
			if(ai->second == "")continue;//FIX ME PLEASE
			aliases_expanded+="alias ";
			aliases_expanded+=((*ai).first);
			aliases_expanded+="=\"";
			aliases_expanded+=((*ai).second);
			aliases_expanded+="\"\n";
		}
		return aliases_expanded;
	}

	fim::string CommandConsole::alias(std::vector<Arg> args)
	{
		/*
		 * assigns to an alias some action
		 */
		fim::string cmdlist;
		if(args.size()==0)
		{
			return displayAliases();
		}
		if(args.size()<2)return "alias : please specify some action to do\n";
		for(int i=1;i<args.size();++i) cmdlist+=args[i].val;
		if(aliases[args[0].val]!="")
		{
			aliases[args[0].val]=cmdlist;
			return fim::string("alias ")+args[0].val+fim::string(" successfully replaced.\n");
		}
		else
		{
			aliases[args[0].val]=cmdlist;
			return fim::string("alias ")+args[0].val+fim::string(" successfully added.\n");
		}
	}

	fim::string CommandConsole::dummy(std::vector<Arg> args)
	{
		std::cout << "dummy function : for test purposes :)\n";
		return "dummy function : for test purposes :)\n";
	}

	fim::string CommandConsole::help(const std::vector<fim::string> &args)
	{	
		/*
		 *	FIX ME
		 */
		Command *cmd;
		if(!args.empty())
		{
			cmd=findCommand(args[0]);
			if(cmd)return  cmd->getHelp()+fim::string("\n");
			else cout << args[0] << " : no such command\n";
		}
		return "usage : help CMD   (use TAB to get a list of commands :) )\n";
	}

	CommandConsole::CommandConsole()
	{	
		dont_record_last_action=false;
		recordMode=false;
		nofb=0;
		rl::initialize_readline();
		cycles=0;isinscript=0;
		addCommand(new Command(fim::string("next" ),fim::string("displays the next picture in the list"),&browser,&Browser::next));
		addCommand(new Command(fim::string("prev" ),fim::string("displays the previous picture in the list"),&browser,&Browser::prev));
		addCommand(new Command(fim::string("push" ),fim::string("pushes a file in the files list"),&browser,&Browser::push));
		addCommand(new Command(fim::string("display" ),fim::string("displays the current file"),&browser,&Browser::display));
		addCommand(new Command(fim::string("redisplay" ),fim::string("re-displays the current file"),&browser,&Browser::redisplay));
		addCommand(new Command(fim::string("list" ),fim::string("displays the files list"),&browser,&Browser::list));
		addCommand(new Command(fim::string("pop"  ),fim::string("pop the last file from the files list"),&browser,&Browser::pop));
		addCommand(new Command(fim::string("file" ),fim::string("displays the current file's name"),&browser,&Browser::get));
		addCommand(new Command(fim::string("panup" ),fim::string("pans the image up"),&browser,&Browser::pan_up));
		addCommand(new Command(fim::string("pandown" ),fim::string("pans the image down"),&browser,&Browser::pan_down));
		addCommand(new Command(fim::string("panleft" ),fim::string("pans the image left"),&browser,&Browser::pan_left));
		addCommand(new Command(fim::string("panright" ),fim::string("pans the image right"),&browser,&Browser::pan_right));
		addCommand(new Command(fim::string("load" ),fim::string("load the image, if not yet loaded"),&browser,&Browser::load));
		addCommand(new Command(fim::string("reload" ),fim::string("loads the image into memory"),&browser,&Browser::reload));
		addCommand(new Command(fim::string("files"),fim::string("displays the number of files in the file list" ),&browser,&Browser::n));
		addCommand(new Command(fim::string("sort" ),fim::string("sorts the file list" ),&browser,&Browser::_sort));
		addCommand(new Command(fim::string("remove" ),fim::string("remove the current file or the selected ones from the list" ),&browser,&Browser::remove));
		addCommand(new Command(fim::string("info" ),fim::string("info about the current file" ),&browser,&Browser::info));
		addCommand(new Command(fim::string("scale_factor_grow" ),fim::string("multiply the scale factors reduce_factor and magnify_factor by scale_factor_multiplier"),&browser,&Browser::scale_factor_increase));
		addCommand(new Command(fim::string("scale_factor_shrink" ),fim::string("divide the scale factors reduce_factor and magnify_facto by scale_factor_multiplier"),&browser,&Browser::scale_factor_decrease));
		addCommand(new Command(fim::string("scale_factor_increase" ),fim::string("add scale_factor_delta to the scale factors reduce_factor and magnify_facto" ),&browser,&Browser::scale_factor_increase));
		addCommand(new Command(fim::string("scale_factor_decrease" ),fim::string( "subtract scale_factor_delta to the scale factors reduce_factor and magnify_factor" ),&browser,&Browser::scale_factor_decrease));
		addCommand(new Command(fim::string("magnify" ),fim::string("magnifies the displayed image" ),&browser,&Browser::magnify));
		addCommand(new Command(fim::string("reduce"),fim::string("reduces the displayed image" ),&browser,&Browser::reduce));
		addCommand(new Command(fim::string("goto"),fim::string("goes to the index image" ),&browser,&Browser::goto_image));
		addCommand(new Command(fim::string("status"),fim::string("sets the status line"),this,&CommandConsole::status));
		addCommand(new Command(fim::string("scrolldown" ),fim::string("scrolls down the image, going next if at bottom" ),&browser,&Browser::scrolldown));
		addCommand(new Command(fim::string("auto_scale" ),fim::string("" ),&browser,&Browser::auto_scale));
		addCommand(new Command(fim::string("auto_width_scale" ),fim::string("scale the image so that it fits horizontally in the screen" ),&browser,&Browser::auto_width_scale));
		addCommand(new Command(fim::string("auto_height_scale" ),fim::string("scale the image so that it fits vertically in the screen" ),&browser,&Browser::auto_height_scale));
		addCommand(new Command(fim::string("bind" ),fim::string("binds some keyboard shortcut to an action"),this,&CommandConsole::bind));
		addCommand(new Command(fim::string("quit"  ),fim::string("terminates the program"),this,&CommandConsole::quit));
		addCommand(new Command(fim::string("exec"  ),fim::string("executes script files"),this,&CommandConsole::executeFile));
		addCommand(new Command(fim::string("echo"  ),fim::string("echoes its arguments"),this,&CommandConsole::echo));
		//addCommand(new Command(fim::string("foo"   ),fim::string("a dummy command"),this,&CommandConsole::foo));
		addCommand(new Command(fim::string("print"   ),fim::string("displays the value of a variable"),this,&CommandConsole::foo));
		addCommand(new Command(fim::string("if"    ),fim::string("if(expression){action;}[else{action;}]"),this,&CommandConsole::foo));
		addCommand(new Command(fim::string("else"    ),fim::string("if(expression){action;}[else{action;}]"),this,&CommandConsole::foo));
		addCommand(new Command(fim::string("while" ),fim::string("while(expression){action;}"),this,&CommandConsole::foo));
		addCommand(new Command(fim::string("alias" ),fim::string("alias ALIAS ACTION"),this,&CommandConsole::foo));
		addCommand(new Command(fim::string("unalias" ),fim::string("unalias ALIAS .."),this,&CommandConsole::unalias));
		addCommand(new Command(fim::string("unbind"),fim::string("unbinds the action associated to KEYCODE"),this,&CommandConsole::unbind));
		addCommand(new Command(fim::string("sleep" ),fim::string("sleeps for n (default 1) seconds"),this,&CommandConsole::foo));
		addCommand(new Command(fim::string("mark" ),fim::string("marks the current file"),this,&CommandConsole::markCurrentFile));
		addCommand(new Command(fim::string("help"  ),fim::string("provides online help"),this,&CommandConsole::help));
		addCommand(new Command(fim::string("autocmd"  ),fim::string("autocommands"),this,&CommandConsole::autocmd));
		addCommand(new Command(fim::string("system"  ),fim::string("system() invocation"),this,&CommandConsole::system));
		addCommand(new Command(fim::string("start_recording"  ),fim::string("starts recording of commands"),this,&CommandConsole::start_recording));
		addCommand(new Command(fim::string("stop_recording"  ),fim::string("stops recording of commands"),this,&CommandConsole::stop_recording));
		addCommand(new Command(fim::string("dump_record_buffer"  ),fim::string("dumps on screen record buffer"),this,&CommandConsole::dump_record_buffer));
		addCommand(new Command(fim::string("execute_record_buffer"  ),fim::string("executes the record buffer"),this,&CommandConsole::execute_record_buffer));
		addCommand(new Command(fim::string("repeat_last"  ),fim::string("repeats the last action"),this,&CommandConsole::repeat_last));
		addCommand(new Command(fim::string("variables"  ),fim::string("displayed the associated variables"),this,&CommandConsole::variables_list));
		addCommand(new Command(fim::string("dump_key_codes"  ),fim::string("dumps the key codes"),this,&CommandConsole::dump_key_codes));
		/*
		 * This is not a nice choice, but it is clean regarding this file.
		 */
		#include "defaultConfiguration.cpp"
	}

	void CommandConsole::init()
	{
		/*
		 *	FIX ME
		 *	Here the program loads the initialization scripts
		 */
//		executeFile("/etc/fim.conf");	//GLOBAL DEFAULT CONFIGURATION FILE
		char rcfile[FIM_MAX_SCRIPT_FILE];
		char *e = getenv("HOME");
		if(e && strlen(e)<128)
		{
			strcpy(rcfile,e);
			strcat(rcfile,"/.fimrc");
			cout << rcfile;
			if(getIntVariable("no_rc_file")==0 )
				executeFile(rcfile);	//GLOBAL DEFAULT CONFIGURATION FILE
		}
		for(int i=0;i<scripts.size();++i) executeFile(scripts[i].c_str());
		//qui ci vorrebbe stat e behaviour condizionale dettata
		//dalla presenza di /etc/fimrc ...
	}

	int CommandConsole::addCommand(Command *c)
	{
		/*
		 * C is added to the commands list
		 */
		assert(c);
		commands.push_back(c);
		return 0;
	}

	fim::string CommandConsole::alias(const fim::string& a,const fim::string& c)
	{
		/*
		 * an internal alias method
		 * FIX ERROR CHECKING
		 */
		std::vector<fim::Arg> args;
		args.push_back(Arg(a));
		args.push_back(Arg(c));
		return alias(args);
	}

	char * CommandConsole::command_generator (const char *text,int state)
	{
		/*
		 *	This is the reason why the commands should be kept
		 *	in a list or vector, rather than a map...  :(
		 *
		 *	TODO : INSTEAD OF USING commands[], make a new vector 
		 *	with completions!
		 *	FIX ME
		 */
		static int list_index=0;
		if(state==0)list_index=0;

		std::vector<fim::string> completions;
		std::map<fim::string,fim::string>::const_iterator ai;
		std::map<fim::string,fim::Var>::const_iterator vi;
		for( ai=aliases.begin();ai!=aliases.end();++ai)
		{
			completions.push_back((*ai).first);
		}
		for( vi=variables.begin();vi!=variables.end();++vi)
		{
//			completions.push_back(fim::string("$")+(*vi).first);
			completions.push_back((*vi).first);
		}
		for(int i=0;i<commands.size();++i)
		{
			completions.push_back(commands[i]->cmd);
		}
		sort(completions.begin(),completions.end());
		
		while(isdigit(*text))text++;	//initial  repeat match
		fim::string cmd(text);

	//	std::vector<fim::string> completions;
//		for(int i=list_index;i<commands.size();++i)
		for(int i=list_index;i<completions.size();++i)
		{
		//	if(!commands[i])continue;
//???????????			if(!completions[i])continue;
//			if(commands[i]->cmd.find(cmd)==0)
			if(completions[i].find(cmd)==0)
			{
				list_index++;
				//std::cout << cmd << " matches with " << commands[i]->cmd<<  "\n";
				//readline will free this strings..
//				return dupstr(commands[i]->cmd.c_str());
				return dupstr(completions[i].c_str());
				//return dupstr((fim::string("$")+completions[i]).c_str());
			}
			else
				;//std::cout << cmd << " no matches with " << commands[i]->cmd<<  "\n";
		}

/*		for(int i=list_index;i<aliases_keys.size();++i)
		{
			if(!commands[i])continue;
			if(commands[i]->cmd.find(cmd)==0)
			{
				list_index++;
				//std::cout << cmd << " matches with " << commands[i]->cmd<<  "\n";
				//readline will free this strings..
				return dupstr(commands[i]->cmd.c_str());
			}
			else
				;//std::cout << cmd << " no matches with " << commands[i]->cmd<<  "\n";
		}*/
		//TO DO : ADD VARIABLE AND ALIAS EXPANSION..
		return NULL;
	}

#define MAXSTRING 4096
#define istrncpy(x,y,z) {strncpy(x,y,z-1);x[z-1]='\0';}
#define ferror(s) {/*fatal error*/printf("%s,%d:%s(please submit this error as a bug!)\n",__FILE__,__LINE__,s);exit(-1);}
#define  	MAXTOCS 128


	char ** CommandConsole::tokenize_(const char *s)
	{
		//questa funzione mi ha fatto bestemmiare un sacco
		/*
		 *	This is not cutting edge parsing, so for the limited
		 *	scope of this program it is enough.
		 */
		const char *b=s,*e;	//e will point to the end of the token or fim::string (a separator or NULL)
		int n;
		char c;
		const char*separators=" -=";
		if(s==NULL) return NULL;
		fim::string str(s);
		fim::string t;
		assert(strlen(s)>=str.length());
		char **tokens;
		if((tokens=(char**)calloc(4,MAXTOCS))==NULL)
			return NULL;
		int ntoks=0;
		b+=strspn(b,separators);	//we skip the first separators
		e=b;
		while( (e+=strcspn(e,separators))>b )
		{
			if((tokens[ntoks]=(char *)calloc(e-b+1,1))==NULL)
			{
				for(int i=ntoks-1;i>=0;--i)
					free(tokens[i]);
				return NULL;
			}
			strncpy(tokens[ntoks],b,e-b);
			tokens[ntoks++][e-b]='\0';
			e+=strspn(e,separators);
			b=e;
		}
		tokens[ntoks]=NULL;
		return tokens;
	}

	fim::string CommandConsole::getBoundAction(const int c)
	{
		return bindings[c];
	}

	void CommandConsole::executeBinding(const int c)
	{
		/*
		 *	Executes the command eventually bound to c.
		 */
		if(bindings[c]!="")
		{
			fim::string cf=current();
			cc.autocmd_exec("PreInteractiveCommand",cf);
			execute(getBoundAction(c).c_str(),0);
			cc.autocmd_exec("PostInteractiveCommand",cf);
		}
	}

	void CommandConsole::execute(const char *ss, int add_history_)
	{
		/*
		 *	This method executes a character string containing a script.
		 *	The second argument specigies whether the command is added or 
		 *	not to the command history buffer.
		 */
		char *s=dupstr(ss);
		//executes a whole line, and stores it in the command history, eventually
		//if(s==NULL){ferror("null command");return;}
		assert(s);
		int r = pipe(pipedesc);
		if(r!=0){ferror("pipe error\n");}
		r=write(pipedesc[1],s,strlen(s));
		close(pipedesc[1]);
		if(r!=strlen(s)){ferror("write error");} 
		for(char*p=s;*p;++p)if(*p=='\n')*p=' ';
//		cout << s << "\n";
		yyparse();
//		printf("\n");
		close(pipedesc[0]);
		//this->execute(fim::string(s));
		if(add_history_)if(nochars(s)==0)add_history(s);
		free(s);
	}

/*	void CommandConsole::execute(fim::string cmd)
	{
		//this method executes a single command without arguments.
		std::cout <<  this->execute(cmd,std::vector<fim::string>());
	}*/

        fim::string CommandConsole::execute(fim::string cmd, std::vector<fim::string> args)
	{
		/*
		 *	This is the method where the tokenized commands are executed.
		 */
		//cout << "about to exec " << cmd.c_str() << "\n";
//		if(cmd=="quit")quit();
//		std::cout << cmd << "\n";
		//this method executes single commands with arguments.
		Command *c=NULL;
		/*
		 * we first determine if this is an alias
		 */
		fim::string ocmd=aliasRecall(cmd);
		if(ocmd!="")
		{
			//an alias should be expanded. arguments are appended.
			fim::string ex;
			cmd=ocmd;
			ex=ocmd;
			/*
			 * WARNING : i am not sure this is harmless
			 */
			int r = pipe(pipedesc),sl;
			if(r!=0){ferror("pipe error\n");exit(-1);}
//			sl=strlen(cmd.c_str());
//			r=write(pipedesc[1],cmd.c_str(),sl);
//			if(r!=sl){ferror("pipe write error");exit(-1);} 
			for(int i=0;i<args.size();++i)
			{
//				if(args[i].c_str()==NULL){ferror("bad arguments.");}
//				r=write(pipedesc[1]," ",1);
//				if(r!= 1){ferror("pipe write error\n");}
//				sl=strlen(args[i].c_str());
//				r=write(pipedesc[1],args[i].c_str(),sl);
//				if(r!=sl){ferror("pipe write error\n");}
				ex+=fim::string(" \""); ex+=args[i];
				ex+=fim::string("\""); 
			}
			sl=strlen(ex.c_str());
			//std::cout << "\"" << ex <<  "\"\n";
			r=write(pipedesc[1],ex.c_str(),sl);
			if(r!=sl){ferror("pipe write error");exit(-1);} 

			close(pipedesc[1]);
			yyparse();
			close(pipedesc[0]);
			return "";
		}
		if(cmd=="sleep")
		{
			//sleeping for an amount of time specified in seconds.
			if(args.size()>0)sleep(atoi(args[0].c_str()));
			else sleep(1); return "";
		}
		if(cmd=="alias")
		{
			//assignment of an alias
			std::vector<Arg> aargs;	//Arg args :P
			if(args.size()){
			for(int i=0;i<args.size();++i)
			{
//				std::cout<<" "<<args[i]<<"\n";
				aargs.push_back(Arg(args[i]));
			}
			//std::cout << this->alias(aargs) << "\n";
			cout << this->alias(aargs) << "\n";
			return "";}
			return displayAliases();
		}
		else
		{
			//if(_commands[cmd]!="")
			if(c=findCommand(cmd))
			{
			//	std::cout << "cmd : '" << cmd << "'\n";
			//	std::cout << "args: " << args << "\n";
			//	std::cout << "`"<<cmd<<" "<<args<<"`\n";
	//			std::cout<<c->execute(args)<<"\n";
				return c->execute(args);
			}
			else
	//			std::cout << "NO `"<<cmd<<" "<<args<<"`\n";
				cout << "NO `"<<cmd.c_str()<<"...\n";
		}
		return "If you see this string, please report it to the program maintainer :P\n";
	}

	int CommandConsole::catchInteractiveCommand()
	{
		/*
		 *	THIS DOES NOT WORK, BECAUSE IT IS A BLOCKING READ.
		 *	MAKE THIS READ UNBLOCKING AN UNCOMMENT.
		 *	
		 *	FIX ME
		 *
		 *	note : this call should 'steal' 1/10 of second..
		 */
		fd_set          set;
		FD_SET(0, &set);

		struct termios tattr;
		struct termios sattr;
		//we set the terminal in raw mode.
		    
	//	fcntl(0,F_GETFL,&saved_fl);
		tcgetattr (0, &sattr);
		    
		//fcntl(0,F_SETFL,O_BLOCK);
		memcpy(&tattr,&sattr,sizeof(struct termios));
		tattr.c_lflag &= ~(ICANON|ECHO);
		tattr.c_cc[VMIN] = 0;
		tattr.c_cc[VTIME] = 1;
		tcsetattr (0, TCSAFLUSH, &tattr);
		
		int c,r;char buf[64];
		r=read(0,&c,4);
		tcsetattr (0, TCSAFLUSH, &sattr);
		return r;
	}

	void CommandConsole::executionCycle()
	{
		cc.autocmd_exec("PreExecutionCycle","<>");
	 	while(1)
		{
			cycles++;
#ifndef FIM_NOFB
/*			if (switch_last != fb_switch_state)
			{
				console_switch(0);
			}*/
			fd_set          set;
			struct timeval  limit;
			FD_SET(0, &set);
			limit.tv_sec = -1;
			limit.tv_usec = 0;
#else
			//ic=true;
#endif
			if(ic)
			{
				char *rl=readline(":");
				if(rl==NULL)
				{
					//printf("rl null\n");
					this->exit(0);
				}
				else if(rl!="")
				{
			/*		if (switch_last != fb_switch_state)
					{
						console_switch(1);
					}*/

					fim::string cf=current();
					cc.autocmd_exec("PreInteractiveCommand",cf);
					if(recordMode)record_action(fim::string(rl));
					ic=0; // we 'exit' from the console for a while
					execute(rl,1);	//execution of the command line with history
					ic=1;
//					execute("redisplay;",0);	//execution of the command line with history
					cc.autocmd_exec("PostInteractiveCommand",cf);
					memorize_last(rl);
					//p.s.:note that current() returns not necessarily the same in 
					//the two autocmd_exec() calls..
				}
				if(rl && *rl=='\0')ic=!ic;
				if(rl)free(rl);
			}
			else
			{
				int c;char buf[64];
				tty_raw();
//				int c=getchar();
//				int c=fgetc(stdin);
				/*
				 *	problems :
				 *	 I can't read Control key and 
				 *	some upper case key together.
				 *	 I am not quite sure about portability..
				 *	... maybe a sample program which photograph
				 *	the keyboard is needed!.
				 */
				c=0;
//				read(0,&c,1); if(c==0x1b){read(0,&c,3);c=(0x1b)+(c<<8);}
				if(read(0,&c,4)>0)	//up to four chars should suffice
				{
//					cout<< "got:           "<<(char*)c<<"(   "  <<c<<")"<<"\n";
					if(getIntVariable("verbose_keys"))
					{
						/*
						 * <0x20 ? print ^ 0x40+..
						 * */
						sprintf(buf,"got : %x (%d)\n",c,c);
						cout << buf ;
					}
					tty_restore();
					if(c==getIntVariable("console_key"))ic=!ic;	//should be configurable..
					else
					{
						this->executeBinding(c);
						if(recordMode) record_action(getBoundAction(c));
						memorize_last(getBoundAction(c));
					}
				}else
				{
					//cout<< "error reading key from keyboard\n";
					/*
					 * 	This happens when console switching, too.
					 * 	So a redraw after is not bad.
					 * 	But it should work when stepping into the console,
					 * 	not out..
					 *
					 * 	PLACE A MECHANISM HERE..
					 */
					//FIX! (this shoul hack the console switch problem)
					//browser.display();
				}
			}
		}
		cc.autocmd_exec("PostExecutionCycle","<>");
	}

	void CommandConsole::exit(int i)
	{
		/*
		 *	This method should be called only when there is no
		 *	potential harm to the console.
		 */
		std::cout<<"\nThe program ended successfully\n";
		std::exit(i);
	}

	void CommandConsole::quit()
	{
		/*
		 * the method to be called to exit from the program safely
		 */
    		cleanup_and_exit(0);
		this->exit(0);
	}

	fim::string CommandConsole::quit(const std::vector<fim::string> &args) { this->quit(); }

	fim::string CommandConsole::executeFile(const std::vector<fim::string> &args)
	{
		for(int i=0;i<args.size();++i)executeFile(args[i].c_str());
		return "";
	}
	
	fim::string CommandConsole::foo(const std::vector<fim::string> &args)
	{
		//std::cout<<"foo ";
		for(int i=0;i<args.size();++i)std::cout<<" "<<args[i];std::cout<<"\n";
	}

	CommandConsole::~CommandConsole()
	{
		/*
		 *	FIX ME :
		 *	PRINTING MARKED FILES..
		 */
		std::cout << "\n";
		for(std::set<fim::string>::iterator i=marked_files.begin();i!=marked_files.end();++i)
			std::cout << *i << "\n";
	}

	int CommandConsole::toggleStatusLine()
	{
		/*
		 * toggles the display of the status line
		 *
		 * FIX ME
		 */
		int sl=getIntVariable("status_line")?0:1;
		setVariable("status_line",sl);
	}

	int CommandConsole::setVariable(const fim::string& varname,int value)
	{
		/*
		 * an internal function to set a user variable
		 */
//		cout << "setVariable " << variables[varname].setFloat(value) << "\n"; 
		//variables[varname]=value;
		return variables[varname].setInt(value);
	}

	float CommandConsole::setVariable(const fim::string& varname,float value)
	{
		/*
		 * an internal function to set a user variable
		 */
//		cout << "setVariable " << variables[varname].setFloat(value) << "\n"; 
		//variables[varname]=value;
		return variables[varname].setFloat(value);
	}
	
	int CommandConsole::executeFile(const char *s)
	{
		/*
		 * FIX ME 
		 * i should warn the user the script file maximum was reached..
		 */
		struct stat stat_s;
		if(!s)return -1;
//		errno=1;		//comment me
		if(-1==stat(s,&stat_s))return -1;
		cout << "executing " << s << "\n";
		int fd,r;
		char buf[4096*32];//FIXME
		fd=open(s,0);
		if(fd+1==0)
		{
			cout << "error opening " << s << "\n";
			return -1; 
		}
		r=read(fd,buf,sizeof(buf)-1);buf[r]='\0';
		cout << "read " << r << " bytes from " << s << "\n";
		buf[min(fim::string::max(),sizeof(buf)-1)]='\0';
//		cout << "\"\n"<<  buf << "\"\n";	//segfaults!
		isinscript=1;
		execute(buf,0);
		isinscript=0;
		close(fd);
///		cout << buf << "\n";	
//		cout << "please note that executeFile is still unfinished..\n";
/*		fd=open("out.log",O_APPEND);
		if(fd+1==0){
			cout << "error opening " << "log.log" << "\n";
			return;
			}
		r=write(fd,buf,r); r=write(fd,"spazzatura",8); close(fd); sync();*/
		return 0;
	}

	fim::string CommandConsole::echo(const std::vector<fim::string> &args)
	{
		/*
		 * a command to echo arguments, for debug and learning purposes
		 */
		if(args.size()==0)fim::cout<<"echo command\n";
		for(int i=0;i<args.size();++i)fim::cout << (args[i].c_str()) << "\n";
		return "";
	}

	int CommandConsole::getVariableType(const fim::string &varname)
	{
		return variables[varname].getType();
	}

	int CommandConsole::getIntVariable(const fim::string &varname)
	{
//		cout << "getVariable " << varname  << " : " << (int)(variables[varname])<< "\n";
		return variables[varname];
	}

	float CommandConsole::getFloatVariable(const fim::string &varname)
	{
//		cout << "getVariable " << varname  << " : " << variables[varname].getFloat()<< "\n";
//		cout << "getVariable " << varname  << ", type : " << variables[varname].getType()<< "\n";
		return variables[varname].getFloat();
	}

	int CommandConsole::drawOutput()
	{
		return inConsole() || this->getIntVariable("verbose");
	}

	std::vector<fim::string> CommandConsole::autocmds_sub_list(const fim::string &event)
	{
		/*
		 * returns the autocommands available for the given event
		 * ( more precisely, the couple (PATTERN,ACTION) )
		 *  DELETE ME 
		 */
		autocmds_p_t::const_iterator api;
		std::vector<fim::string> sub_list;
	/*	for( api=autocmds[event].begin();api!=autocmds[event].end();++api )
		{
			acl+=((*ai).first);
			acl+=" ";
		}*/
		return sub_list;
	}

	fim::string CommandConsole::get_variables_list()
	{
		/*
		 * FIX ME
		 */
		fim::string acl;
		std::map<fim::string,fim::Var>::const_iterator vi;
		for( vi=variables.begin();vi!=variables.end();++vi)
		{
			acl+=((*vi).first);
			acl+=" ";
		}
		return acl;
	}

	fim::string CommandConsole::autocmds_list()
	{
		/*
		 * as of now, lists the events for which an autocmd could be assigned.
		 *
		 * FIX ME
		 */
		fim::string acl;
//		std::map<fim::string,std::map<fim::string,fim::string> >  autocmds;
		autocmds_t::const_iterator ai;
		//for each autocommand event registered
		for( ai=autocmds.begin();ai!=autocmds.end();++ai )
		//for each file pattern registered, display the list..
		for(	autocmds_p_t::const_iterator api=((*ai)).second.begin();
				api!=((*ai)).second.end();++api )
		//.. display the list of autocommands...
		for(	std::vector<fim::string>::const_iterator aui=((*api)).second.begin();
				aui!=((*api)).second.end();++aui )
		{
			acl+="autocmd \""; 
			acl+=(*ai).first; 
			acl+="\" \""; 
			acl+=(*api).first; 
			acl+="\" \""; 
			acl+=(*aui); 
			acl+="\"\n"; 
		}
		return acl;
	}

	fim::string CommandConsole::autocmd(const std::vector<fim::string>& args)
	{
		/*
		 * associates an action to a certain event in certain circumstances
		 */
		fim::string usage="usage: autocmd Event Pattern Commands\n";
		//cout << "autocmd '"<<args[0]<<"' '"<<args[1]<<"' '"<<args[2]<<"' added..\n";
		if(args.size()==0)
		{
			return autocmds_list();
		}
		if(args.size()==1 || args.size()>3)
		{
			return usage;
		}
		if(args.size()==2)
		{
			return autocmd_del(args[0],args[1]);
		}
		if(args.size()==3)
		{
//			cout << "autocmd '"<<args[0]<<"' '"<<args[1]<<"' '"<<args[2]<<"' added..\n";
			return autocmd_add(args[0],args[1],args[2]);
		}
		return "";
	}

	fim::string CommandConsole::autocmd_add(const fim::string &event,const fim::string &pat,const fim::string &cmd)
	{
		/*
		 * the internal autocommand add function
		 *
		 * TODO : VALID VS INVALID EVENTS?
		 */
		autocmds[event][pat].push_back(cmd);
		return "";
	}

	fim::string CommandConsole::autocmd_exec(const fim::string &event,const fim::string &fname)
	{
		/*
		 *	WARNING : maybe there is the need of a sandbox, for
		 *	any command could do harm to the iterators themselves!
		 *
		 *	SO THE FIRST MATCHING SHOULD RETURN!
		 */
//		cout << "autocmd_exec..\n";
		autocmds_p_t::const_iterator api;
		/*
		 *	we want to prevent from looping autocommands, so this rudimentary
		 *	mechanism should avoid the majority of them.
		 */
		if(! autocmd_in_stack( autocmds_frame_t(event,fname)))
		{
			autocmd_push_stack( autocmds_frame_t(event,fname));
			for( api=autocmds[event].begin();api!=autocmds[event].end();++api )
			{
				autocmd_exec(event,(*api).first,fname);
			}
			autocmd_pop_stack(autocmds_frame_t(event,fname));
		}
		else
		{
			cout << "WARNING : there is a loop for "
			     << "(event:" << event << ",filename:" << fname << ")";
		}
		return "";
	}

	fim::string CommandConsole::autocmd_exec(const fim::string &event,const fim::string &pat,const fim::string &fname)
	{
		/*
		 * executes all the actions associated to the current event, if the current 
		 * file name matches the individual patterns
		 *
		 * FIX ME : REGEXP MATCHING SHOULD BE MOVED BEFORE !
		 */
//		cout << "autocmd_exec_cmd...\n";
//		cout << "autocmd_exec_cmd. for pat '" << fname <<  "'\n";
		for (int i=0;i<autocmds[event][pat].size();++i)
		{
			if(regexp_match(fname.c_str(),pat.c_str()))	//UNFINISHED : if fname matches path pattern.. now matches ALWAYS
			{
//				cout << "should exec '"<<event<<"'->'"<<autocmds[event][pat][i]<<"'\n";
				execute((autocmds[event][pat][i]).c_str(),0);
			}
		}
		return "";
	}

	void CommandConsole::display()
	{
		/*
		 * quick and dirty display function
		 */
		browser.display();
	}

	void CommandConsole::autocmd_push_stack(const autocmds_frame_t& frame)
	{
		//WARNING : ERROR DETECTION IS MISSING
		autocmds_stack.insert(frame);
	}

	void CommandConsole::autocmd_pop_stack(const autocmds_frame_t& frame)
	{
		//WARNING : ERROR DETECTION IS MISSING
		autocmds_stack.erase(frame);
	}
	
	int CommandConsole::autocmd_in_stack(const autocmds_frame_t& frame)
	{
		/*
		 * this function prevents a second autocommand triggered against 
		 * the same file to execute
		 */
		return  autocmds_stack.find(frame)!=autocmds_stack.end();
	}
	
	bool CommandConsole::regexp_match(const char*s, const char*r)const
	{
		/*
		 *	FIX ME
		 */
		regex_t regex;	//should be static!!!
		const int nmatch=1;
		regmatch_t pmatch[nmatch];

		/*
		 * we allow for the default match, in case of null regexp
		 */
		if(!r || !strlen(r))return true;

		//if(regcomp(&regex,"^ \\+$", 0 | REG_EXTENDED | REG_ICASE )==-1)
		if(regcomp(&regex,r, 0 | REG_EXTENDED | REG_ICASE )==-1)
		{
			cout << "error calling regcomp (invalid regexp?)!" << "\n";
			return false;
		}
		else
		{
//			cout << "done calling regcomp!" << "\n";
		}
		//if(regexec(&regex,s+0,nmatch,pmatch,0)==0)
		if(regexec(&regex,s+0,nmatch,pmatch,0)!=REG_NOMATCH)
		{
//			cout << "'"<< s << "' matches with '" << r << "'\n";
/*			cout << "match : " << "\n";
			cout << "\"";
			for(int m=pmatch[0].rm_so;m<pmatch[0].rm_eo;++m)
				cout << s[0+m];
			cout << "\"\n";*/
			regfree(&regex);
			return true;
		}
		else
		{
		//	cout << "no match" << "\n";
		};
		regfree(&regex);
		return false;
		return true;
	}

	fim::string CommandConsole::system(const std::vector<fim::string>& args)
	{
		for(int i=0;i<args.size();++i)
		{
			std::system(args[i].c_str());
		}
		return "";
	}
	
	fim::string CommandConsole::status(const std::vector<fim::string> &args)
	{
		for(int i=0;i<args.size();++i)
		{
			browser.display_status(args[i].c_str(),NULL);
		}
		return "";
	}

	fim::string CommandConsole::unalias(const std::vector<fim::string>& args)
	{
		/*
		 * removes the binding from an alias to the associated action
		 */
		if(args.size()<1)return "unalias : please specify an alias to remove!\n";
		for(int i=0;i<args.size();++i)
		if(aliases[args[i]]!="")
		{
			aliases.erase(args[i]);
			return "";//fim::string("unalias : \"")+args[i]+fim::string("\" successfully unaliased.\n");
		}else
			return fim::string("unalias : \"")+args[i]+fim::string("\" there is not such alias.\n");
	}

	fim::string CommandConsole::dump_key_codes(const std::vector<fim::string>& args)
	{
		fim::string acl;
		std::map<fim::string,int>::const_iterator ki;
		for( ki=key_bindings.begin();ki!=key_bindings.end();++ki)
		{
			acl+=((*ki).first);
			acl+=" -> ";
			acl+=((*ki).second);
			acl+=", ";
		}
		return acl;
	}
};
