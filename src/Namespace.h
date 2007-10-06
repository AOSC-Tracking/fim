/* $Id$ */
/*
 Namespace.h : Namespace class headers

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

#include "fim.h"

#ifndef NAMESPACE_FBI_H
#define NAMESPACE_FBI_H

namespace fim
{
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
		std::map<const fim::string,Var> variables;	//id->var
	
		public:
		int setVariable(const fim::string& varname,int value)
		{
			return variables[varname].setInt(value);
		}

		float setVariable(const fim::string& varname,float value)
		{
			/*
			 * an internal function to set a user variable
			 */
	//		cout << "setVariable " << variables[varname].setFloat(value) << "\n"; 
			return variables[varname].setFloat(value);
		}

		int setVariable(const fim::string& varname,const char*value)
		{
			/*
			 * an internal function to set a user variable
			 */
			fim::string s(value);
			return (int)(variables[varname].setString(s));
		}
	
		int getIntVariable(const fim::string &varname)
		{
			// this scope was selected
			return variables[varname];
		}

		float getFloatVariable(const fim::string &varname)
		{
			/*
			 * the variable name supplied is used as a key to the variables hash
			 * */
//			cout << "getVariable " << varname  << " : " << variables[varname].getFloat()<< "\n";
//			cout << "getVariable " << varname  << ", type : " << variables[varname].getType()<< "\n";
			return variables[varname].getFloat();
		}

		fim::string getStringVariable(const fim::string &varname)
		{
			/*
			 * the variable name supplied is used as a key to the variables hash
			 * */
			return variables[varname].getString();
		}
	};
}

#endif

