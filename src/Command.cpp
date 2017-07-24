/* $LastChangedDate$ */
/*
 Command.cpp :

 (c) 2007-2013 Michele Martone

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
#include "fim.h"
#include "Command.h"
namespace fim
{
	fim::string Command::execute(const args_t&args)
	{
		assert(browser_ && browserf_);
		return (browser_->*browserf_)(args); // Browser or CommandConsole or FimWindow
	}
	const fim_cmd_id & Command::cmd(void)const{return cmd_;}
	const fim::string & Command::getHelp(void)const{return help_;}
	Command::Command(fim_cmd_id cmd, fim::string help, Browser *b, fim::string(Browser::*bf)(const args_t&))
	       	:cmd_(cmd),help_(help),browserf_(bf),browser_(b) {}
} /* namespace fim */
