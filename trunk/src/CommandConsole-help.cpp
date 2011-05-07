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
		/*variables_t::const_iterator vi;
		for( vi=variables_.begin();vi!=variables_.end();++vi)
		{
			s+=vi->first;
			s+=" : ";
			s+=Var::var_help_db_query(vi->first);
			s+="\n";
		}*/
		s+= Var::get_variables_reference(refmode);
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
			string("\n")+
			string(".SH COMMANDS REFERENCE\n")+
			string("\n")+
			get_commands_reference(refmode)+
			string(".SH VARIABLES REFERENCE\n")+
			string("\n")+
		       	get_variables_reference(refmode);
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

