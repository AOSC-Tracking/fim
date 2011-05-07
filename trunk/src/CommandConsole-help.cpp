/* $Id$ */
/*
 CommandConsole-help.cpp : Fim console dispatcher--help methods

 (c) 2011 Michele Martone

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

namespace fim
{
	fim::string CommandConsole::get_variables_reference(FimDocRefMode refmode)const
	{
		/*
		 * returns the reference of registered functions
		 */
		fim::string s;
		fim::string sep=" ";
		s+= Var::get_variables_reference(refmode);

		// FIXME: shall fix the following to work
#if 0
#ifdef FIM_NAMESPACES
		s+=browser_.get_variables_list();
		s+=sep;
		if(browser_.c_image())
		{
			s+=browser_.c_image()->get_variables_list();
			s+=sep;
		}
#endif
#endif
#if 0
#ifdef FIM_WINDOWS
		if(window_)
		{
			s+=window_->get_variables_list();
			s+=sep;
		}
		if(current_viewport())
		{
			s+=current_viewport()->get_variables_list();
			s+=sep;
		}
#endif
#endif
		// FIXME: incomplete
		return s;
	}

	fim::string CommandConsole::get_commands_reference(FimDocRefMode refmode)const
	{
		/*
		 * returns the reference of registered commands_
		 * TODO : should write better help messages
		 */
		fim::string s;
		if(refmode==Man)
			goto manmode;
		for(size_t i=0;i<commands_.size();++i)
		{
			s+=(commands_[i]->cmd_);
			s+=" : ";
			s+=(commands_[i])->getHelp();
			s+="\n";
		}
		return s;
manmode:
		for(size_t i=0;i<commands_.size();++i)
		{
			s+=".B\n";
			s+=(commands_[i]->cmd_);
			s+="\n";
			s+=(commands_[i])->getHelp();
			s+="\n";
			s+=".fi\n";
		}
		return s;
	}

	fim::string CommandConsole::print_commands()const
	{
		cout << "VARIABLES : "<<get_variables_list()<<"\n";
		cout << "COMMANDS : "<<get_commands_list()<<"\n";
		cout << "ALIASES : "<<get_aliases_list()<<"\n";
		return "";
	}

	fim::string CommandConsole::get_reference_manual(const args_t& args)
	{
		/*
		 * dump textually a reference manual from all the available fim language features.
		 */
		FimDocRefMode refmode=DefRefMode;
		if(args.size()==1 && args[0]=="man")
		{
			refmode=Man;
			return
			string(".\\\"\n")+
			string(".\\\" $Id$\n")+
			string(".\\\"\n")+
			string(".TH fimrc 1 \"(c) 2011-2011 Michele Martone\"\n")+
			string(".SH NAME\n")+
			string("fimrc - \\fB fim \\fP configuration file and language reference\n")+
			string("\n")+
			string(".SH SYNOPSIS\n")+
			string(".B ~/fimrc\n.fi\n")+
			string(".B /etc/fimrc\n.fi\n")+
			string(".B fim --"FIM_OSW_SCRIPT_FROM_STDIN" [ options ] < {scriptfile}\n.fi\n")+
			string(".B fim --"FIM_OSW_EXECUTE_SCRIPT" {scriptfile} [ options ]\n.fi\n")+
			string(".B fim --"FIM_OSW_EXECUTE_COMMANDS" {commands} [ options ]\n.fi\n")+
			string(".B fim --"FIM_OSW_FINAL_COMMANDS" {commands} [ options ]\n.fi\n")+
			string(".B fim --"FIM_OSW_DUMP_SCRIPTOUT" {scriptfile} [ options ]  \n.fi\n")+
			string(".B fim --"FIM_OSW_DUMP_SCRIPTOUT" "FIM_LINUX_STDOUT_FILE" [ options ]\n.fi\n")+
			string("\n")+
			string(".SH DESCRIPTION\n")+
			string("This page explains the \n.B fim\nlanguage, which is used for the \n.B fimrc\nconfiguration files, {scriptfile}s, or {commands} passed via command line options.\n")+
			string("This language can be used to issue commands from the internal program command line accessed interactively through the "FIM_SYM_CONSOLE_KEY_STR" key.\n")+
			string("\n")+
			string("\n")+
			string(".SH FIM LANGUAGE\n")+
			string("Incomplete section.\n")+
			string("\n")+
			string(".SH COMMANDS REFERENCE\n")+
			string("Incomplete section.\n")+
			string("\n")+
			get_commands_reference(refmode)+
			string(".SH VARIABLES REFERENCE\n")+
			string("Incomplete section.\n")+
			string("\n")+
		       	get_variables_reference(refmode)+
			string(".SH EXAMPLES\n")+
			string("Incomplete section.\n")+
			string(".SH SEE ALSO\n")+
			string("fim(1).\n")+
			string(".SH AUTHOR\n")+
			string(FIM_AUTHOR)+
			string("\n")+
			string(".SH COPYRIGHT\n")+
			string("See fim(1)\n")+
			string("\n")+
			string("\n")
			;
		}
		else
		return
			string("commands:\n")+
			get_commands_reference()+
			string("variables:\n")+
			get_variables_reference();
	}

	fim::string CommandConsole::dump_reference_manual(const args_t& args)
	{
		/*
		 * dump textually a reference manual from all the available fim language features.
		 */
		std::cout << get_reference_manual(args);
		return "";
	}
}

