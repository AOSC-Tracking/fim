/* $LastChangedDate$ */
/*
 Command.h : Fim Command class header file

 (c) 2007-2017 Michele Martone

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
#ifndef FIM_COMMAND_H
#define FIM_COMMAND_H
#include "fim.h"
namespace fim
{

#if FIM_USE_CXX11
	using fim_cmd_id = fim::string; // command id
	using fim_cls = fim::string; // command line statement
#else /* FIM_USE_CXX11 */
	typedef fim::string fim_cmd_id; // command id
	typedef fim::string fim_cls; // command line statement
#endif /* FIM_USE_CXX11 */
class Command FIM_FINAL
{
	fim_cmd_id cmd_;
	fim::string help_;
	public:
	const fim_cmd_id & cmd(void)const;
	explicit Command(fim_cmd_id cmd, fim::string help, Browser *b=FIM_NULL, fim::string(Browser::*bf)(const args_t&)=FIM_NULL);
	explicit Command(fim_cmd_id cmd, fim::string help, CommandConsole *c=FIM_NULL,fim::string(CommandConsole::*cf)(const args_t&)=FIM_NULL) :cmd_(cmd),help_(help),consolef_(cf),commandconsole_(c) {}
#ifdef FIM_WINDOWS
	explicit Command(fim_cmd_id cmd, fim::string help, FimWindow *w=FIM_NULL, fim::string(FimWindow::*cf)(const args_t&)=FIM_NULL) :cmd_(cmd),help_(help),windowf_(cf),window_(w) {}
#endif /* FIM_WINDOWS */
	const fim::string & getHelp(void)const;
	private:
	union{
		fim::string (Browser::*browserf_)(const args_t&) ;
		fim::string (CommandConsole::*consolef_)(const args_t&) ;
#ifdef FIM_WINDOWS
		fim::string (FimWindow::*windowf_)(const args_t&) ;
#endif /* FIM_WINDOWS */
	};
	union{
		Browser *browser_;
		CommandConsole *commandconsole_;
#ifdef FIM_WINDOWS
		FimWindow *window_;
#endif /* FIM_WINDOWS */
	};
	fim_cmd_type_t type;
	public:
	~Command(void) { }
	fim::string execute(const args_t&args);
	bool operator < (const Command&c)const{return cmd_< c.cmd_;}
	bool operator <=(const Command&c)const{return cmd_<=c.cmd_;}
};
} /* namespace fim */
#endif /* FIM_COMMAND_H */
