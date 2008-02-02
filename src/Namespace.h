/* $Id$ */
/*
 Namespace.h : Namespace class headers

 (c) 2007-2008 Michele Martone

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

#ifndef NAMESPACE_FBI_H
#define NAMESPACE_FBI_H

#include "fim.h"

namespace fim
{
	extern CommandConsole cc;
	typedef std::map<const fim::string,Var> variables_t;	//id->var
	class Namespace
	{
		/*
		 * FIXME: experimental
		 *
		 * note that right here inside there is no check on the id token correctness 
		 * respect to the Fim language grammar syntax.
		 *
		 * the identifier->variable binding
		 */
		variables_t variables;	//id->var
	
		public:
		/*
		variables_t getVariables()const
		{
			return variables;
		}*/

		int setVariable(const fim::string& varname,int value);
		float setVariable(const fim::string& varname,float value);
		int setVariable(const fim::string& varname,const char*value);

		int getIntVariable(const fim::string &varname)const;
		float getFloatVariable(const fim::string &varname);
		fim::string getStringVariable(const fim::string &varname);

		int  setGlobalVariable(const fim::string& varname,int value);
	        float setGlobalVariable(const fim::string& varname,float value);
		int setGlobalVariable(const fim::string& varname,const char*value);

		int getGlobalIntVariable(const fim::string &varname)const;
		float getGlobalFloatVariable(const fim::string &varname);
		fim::string getGlobalStringVariable(const fim::string &varname);
		fim::string autocmd_exec(const fim::string &event,const fim::string &fname);
	};
}

#endif

