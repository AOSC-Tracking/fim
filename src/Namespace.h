/* $LastChangedDate$ */
/*
 Namespace.h : Namespace class headers

 (c) 2007-2016 Michele Martone

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
#if FIM_USE_CXX11
	using fim_var_id = fim::string ;	//id
	using variables_t = std::map<const fim_var_id,Var> ;	//id->var
	//using fim_var_id_list = std::vector<fim_var_id> ;
	using fim_var_id_set = std::set<fim_var_id> ;
	using fim_var_val_set = std::set<Var> ;
#else /* FIM_USE_CXX11 */
	typedef fim::string fim_var_id;	//id
	typedef std::map<const fim_var_id,Var> variables_t;	//id->var
	//typedef std::vector<fim_var_id> fim_var_id_list;
	typedef std::set<fim_var_id> fim_var_id_set;
	typedef std::set<Var> fim_var_val_set;
#endif /* FIM_USE_CXX11 */

	class Namespace
	{
		protected:
#ifndef FIM_INDEPENDENT_NAMESPACE
		CommandConsole*rnsp_; // root Namespace pointer
#endif /* FIM_INDEPENDENT_NAMESPACE */
		variables_t variables_;	//id->var
		fim_char_t ns_char_; // ns_char_ ':' varname
	
		public:

		fim_int setVariable(const fim_var_id& varname,fim_int value);
		fim_float_t setVariable(const fim_var_id& varname,fim_float_t value);
		fim_int setVariable(const fim_var_id& varname,const fim_char_t* value);
		Var setVariable(const fim_var_id& varname,const Var& value);
		Var setVariable(const fim_var_id& varname,const fim::string& value);

		fim_int getIntVariable(const fim_var_id& varname)const;
		fim_float_t getFloatVariable(const fim_var_id& varname)const;
		fim::string getStringVariable(const fim_var_id& varname)const;
		Var getVariable(const fim_var_id& varname)const;

		fim_bool_t isSetVar(const fim_var_id& varname)const;
		void unsetVariable(const fim_var_id& varname);

		fim_int  setGlobalVariable(const fim_var_id& varname,fim_int value);
	        fim_float_t setGlobalVariable(const fim_var_id& varname,fim_float_t value);
		fim_int setGlobalVariable(const fim_var_id& varname,const fim_char_t* value);
		fim_int setGlobalVariable(const fim_var_id& varname, const fim::string& value);

		fim_int getGlobalIntVariable(const fim_var_id& varname)const;
		fim_float_t getGlobalFloatVariable(const fim_var_id& varname)const;
		fim::string getGlobalStringVariable(const fim_var_id& varname)const;
		fim::string autocmd_exec(const fim::string& event, const fim_fn_t& fname);
		fim::string get_variables_list(bool with_values=false, bool fordesc=false)const;
		virtual size_t byte_size(void)const = 0;
		fim_err_t assign_ns(const Namespace& ns);

		Namespace(
#ifndef FIM_INDEPENDENT_NAMESPACE
				CommandConsole *rnsp = FIM_NULL,
#endif /* FIM_INDEPENDENT_NAMESPACE */
			       	const fim_char_t ns_char = FIM_SYM_NULL_NAMESPACE_CHAR
			)
			:
#ifndef FIM_INDEPENDENT_NAMESPACE
		      rnsp_(rnsp),
#endif /* FIM_INDEPENDENT_NAMESPACE */
			variables_(variables_t())
			,ns_char_(ns_char)
	       	{}
		virtual ~Namespace(void){}
		fim_err_t find_matching_list(fim_cmd_id cmd, args_t& completions, bool prepend_ns)const;
		std::ostream& print(std::ostream& os)const;
		void get_id_list(fim_var_id_set& set)const
		{
			for(variables_t::const_iterator fit=variables_.begin();fit!=variables_.end();++fit)
				set.insert(fit->first);
		}
	};
	std::ostream& operator<<(std::ostream& os, const Namespace & ns);
}

#endif /* FIM_NAMESPACE_H */

