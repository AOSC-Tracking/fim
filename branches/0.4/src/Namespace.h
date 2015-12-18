/* $LastChangedDate$ */
/*
 Namespace.h : Namespace class headers

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

#ifndef FIM_NAMESPACE_H
#define FIM_NAMESPACE_H

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
		private:
		variables_t variables_;	//id->var
		fim_char_t ns_char_; // ns_char_ ':' varname
	
		public:

		fim_int setVariable(const fim::string& varname,fim_int value);
		fim_float_t setVariable(const fim::string& varname,fim_float_t value);
		fim_int setVariable(const fim::string& varname,const fim_char_t*value);
		Var setVariable(const fim::string& varname,const Var&value);

		fim_int getIntVariable(const fim::string &varname)const;
		fim_float_t getFloatVariable(const fim::string &varname)const;
		fim::string getStringVariable(const fim::string &varname)const;
		Var getVariable(const fim::string &varname)const;

		fim_int  setGlobalVariable(const fim::string& varname,fim_int value);
	        fim_float_t setGlobalVariable(const fim::string& varname,fim_float_t value);
		fim_int setGlobalVariable(const fim::string& varname,const fim_char_t*value);

		fim_int getGlobalIntVariable(const fim::string &varname)const;
		fim_float_t getGlobalFloatVariable(const fim::string &varname)const;
		fim::string getGlobalStringVariable(const fim::string &varname)const;
		fim::string autocmd_exec(const fim::string &event,const fim::string &fname);
		fim::string get_variables_list()const;

		Namespace(const fim_char_t ns_char=FIM_SYM_NULL_NAMESPACE_CHAR):variables_(variables_t()),ns_char_(ns_char) {}
		virtual ~Namespace(){}
		fim_err_t find_matching_list(fim::string cmd, args_t & completions, bool prepend_ns)const;
	};
}

#endif /* FIM_NAMESPACE_H */
