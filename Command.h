#ifndef COMMAND_FBVI_H
#define COMMAND_FBVI_H
#include "fim.h"
namespace fim
{

class Command
{
	enum {BrowserT,CommandConsoleT};
	int type;
	union{
		Browser *browser;
		CommandConsole *console;
	};

	union{
		fim::string (Browser::*browserf)(const std::vector<fim::string>&) ;
		fim::string (CommandConsole::*consolef)(const std::vector<fim::string>&) ;
	};
	private:
	public:
	fim::string cmd;
	fim::string help;
	public:
	fim::string Command::getHelp()const{return help;}
	Command::Command(fim::string cmd,fim::string help,Browser *b=NULL,fim::string(Browser::*bf)(const std::vector<fim::string>&)=NULL) :cmd(cmd),help(help),browserf(bf),browser(b) { type=BrowserT;}
	Command::Command(fim::string cmd,fim::string help,CommandConsole *c=NULL,fim::string(CommandConsole::*cf)(const std::vector<fim::string>&)=NULL) :cmd(cmd),help(help),consolef(cf),console(c) { type=CommandConsoleT;}
	
	Command::~Command() { }
	
/*	fim::string Command::execute(const std::vector<Arg> &args)
	{
//		assert(browser && browserf);
//		(browser->*browserf)(args);	
		assert(0);
//		std::cout<<"ddddaaaaaah\n";
		return "not ok.";
	}*/

	fim::string Command::execute(const std::vector<fim::string> &args)
	{
	//	assert(browser && browserf);
		assert(browser);//!!!!!!!!!!!!!!!!!!
		//std::cerr <<  "about to execute '"<<cmd.c_str();for(int i=0;i<args.size();++i)std::cerr << " " << args[i].c_str(); std::cerr << "'\n";
		if(!browser || !browserf)
		{
			//std::cout << cmd.c_str() << " : "<< "\n";
			return "problems executing command..";
		}else
		return (browser->*browserf)(args);
	}

	bool operator < (Command c)const{return cmd< c.cmd;}
	bool operator <=(Command c)const{return cmd<=c.cmd;}
};
};
#endif
