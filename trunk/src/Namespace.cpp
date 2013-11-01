/* $LastChangedDate$ */
/*
 Namespace.h : a class for local variables storage

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

namespace fim
{


		fim_int Namespace::setVariable(const fim::string& varname,fim_int value)
		{
			return variables_[varname].setInt(value);
		}

		fim_float_t Namespace::setVariable(const fim::string& varname,fim_float_t value)
		{
			/*
			 * an internal function to set a user variable
			 */
	//		cout << "setVariable " << variables_[varname].setFloat(value) << "\n"; 
			return variables_[varname].setFloat(value);
		}

		Var Namespace::setVariable(const fim::string& varname,const Var&value)
		{
			/*
			 * an internal function to set a user variable
			 */
			/* FIXME ! */
//			std::cout << "NSSV:"<<varname<<"\n";
//			std::cout << __FILE__<<":"<<__LINE__<<"\n";
			variables_[varname]=Var(value);
			variables_[varname].set(value);/* FIXME : it seems necessary (see tests) */
//			std::cout << "NSSV:"<<variables_[varname].getString()<<"\n";
			return variables_[varname].getString();
		}

		fim_int Namespace::setVariable(const fim::string& varname,const fim_char_t*value)
		{
			/*
			 * an internal function to set a user variable
			 */
			fim::string s(value);
			return (fim_int)(variables_[varname].setString(s));
		}
	
		fim_int Namespace::getIntVariable(const fim::string &varname)const
		{
			// this scope was selected
			//return variables_[varname];
			variables_t::const_iterator vi=variables_.find(varname);
			fim_int retval = 0;

			if(vi!=variables_.end())
				retval = vi->second.getInt();
			return retval;
		}

		Var Namespace::getVariable(const fim::string &varname)const
		{
			// this scope was selected
			variables_t::const_iterator vi=variables_.find(varname);

			if(vi!=variables_.end())
				return vi->second;
			else
			       	return Var(0);
		}

		fim_float_t Namespace::getFloatVariable(const fim::string &varname)const
		{
			/*
			 * the variable name supplied is used as a key to the variables hash
			 * */
//			cout << "getVariable " << varname  << " : " << variables_[varname].getFloat()<< "\n";
//			cout << "getVariable " << varname  << ", type : " << variables_[varname].getType()<< "\n";
			variables_t::const_iterator vi=variables_.find(varname);
			fim_float_t retval = FIM_CNS_EMPTY_FP_VAL;

			if(vi!=variables_.end())
			       	retval = vi->second.getString();
			return retval;
		}

		fim::string Namespace::getStringVariable(const fim::string &varname)const
		{
			/*
			 * the variable name supplied is used as a key to the variables hash
			 * */
//			std::cout << "NSGSV:"<<varname<<"\n";
			fim::string retval = FIM_CNS_EMPTY_RESULT;
			variables_t::const_iterator vi=variables_.find(varname);
			if(vi!=variables_.end())
			{
//				std::cout << "NSGSV:"<<vi->second.getString()<<"\n";
				retval = vi->second.getString();
			}
			return retval;
		}

	        fim_float_t Namespace::setGlobalVariable(const fim::string& varname,fim_float_t value)
		{
			return cc.setVariable(varname,value);
		}

		fim_int Namespace::setGlobalVariable(const fim::string& varname,fim_int value)
		{
			return cc.setVariable(varname,value);
		}

		fim_int Namespace::setGlobalVariable(const fim::string& varname,const fim_char_t*value)
		{
			return cc.setVariable(varname,value);
		}

		fim_int Namespace::getGlobalIntVariable(const fim::string &varname)const
		{
			return cc.getIntVariable(varname);
		}

		fim_float_t Namespace::getGlobalFloatVariable(const fim::string &varname)const
		{
			return cc.getFloatVariable(varname);
		}

		fim::string Namespace::getGlobalStringVariable(const fim::string &varname)const
		{
			return cc.getStringVariable(varname);
		}

		fim::string Namespace::autocmd_exec(const fim::string &event,const fim::string &fname)
		{
#ifdef FIM_AUTOCMDS
			return cc.autocmd_exec(event,fname);
#else /* FIM_AUTOCMDS */
			return FIM_CNS_EMPTY_RESULT;
#endif /* FIM_AUTOCMDS */
		}

		fim::string Namespace::get_variables_list(void)const
		{
			/*
			 * returns the list of set variables
			 * TODO: to accept an optional argument as separator.
			 */
			fim::string acl;
			variables_t::const_iterator vi;

			for( vi=variables_.begin();vi!=variables_.end();++vi)
			{
				if(ns_char_!=FIM_SYM_NULL_NAMESPACE_CHAR)
				{
					acl+=ns_char_;
					acl+=FIM_SYM_NAMESPACE_SEP;
				}
				acl+=((*vi).first);
				acl+=" ";
			}
			return acl;
		}

		fim_err_t Namespace::find_matching_list(fim::string cmd, args_t & completions, bool prepend_ns)const
		{
			for(variables_t::const_iterator vi=variables_.begin();vi!=variables_.end();++vi)
			{
				if((vi->first).find(cmd)==0)
				{
					fim::string res;
					if(prepend_ns)
						res+=ns_char_,res+=FIM_SYM_NAMESPACE_SEP;
					res+=(*vi).first;
					completions.push_back(res);
				}
			}
			return FIM_ERR_NO_ERROR;
		}
}

