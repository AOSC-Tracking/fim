/* $Id$ */
/*
 Command.h : Fim Command class header file

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
#ifndef COMMAND_FBVI_H
#define COMMAND_FBVI_H
#include "fim.h"
namespace fim
{

class Command
{
	public:
	fim::string cmd,
		    help ;
	Command(fim::string cmd,fim::string help,Browser *b=NULL,fim::string(Browser::*bf)(const std::vector<fim::string>&)=NULL) :cmd(cmd),help(help),browserf(bf),browser(b) { type=BrowserT;}
	Command(fim::string cmd,fim::string help,CommandConsole *c=NULL,fim::string(CommandConsole::*cf)(const std::vector<fim::string>&)=NULL) :cmd(cmd),help(help),consolef(cf),console(c) { type=CommandConsoleT;}

	fim::string getHelp()const{return help;}
	private:
	enum	{ BrowserT,CommandConsoleT };
	int type;
	union{
		fim::string (Browser::*browserf)(const std::vector<fim::string>&) ;
		fim::string (CommandConsole::*consolef)(const std::vector<fim::string>&) ;
	};
	union{
		Browser *browser;
		CommandConsole *console;
	};

	public:
	
	~Command() { }
	
/*	fim::string execute(const std::vector<Arg> &args)
	{
//		assert(browser && browserf);
//		(browser->*browserf)(args);	
		assert(0);
//		std::cout<<"ddddaaaaaah\n";
		return "not ok.";
	}*/

	fim::string execute(const std::vector<fim::string> &args)
	{
	//	assert(browser && browserf);
		assert(browser);//!!!!!!!!!!!!!!!!!!
		//std::cerr <<  "about to execute '"<<cmd.c_str();for(int i=0;i<args.size();++i)std::cerr << " " << args[i].c_str(); std::cerr << "'\n";
		if(!browser || !browserf)
		{
			//std::cout << cmd.c_str() << " : "<< "\n";
			return fim::string("problems executing command..");
		}else
		return (browser->*browserf)(args);
	}

	bool operator < (Command c)const{return cmd< c.cmd;}
	bool operator <=(Command c)const{return cmd<=c.cmd;}
};
}
#endif
