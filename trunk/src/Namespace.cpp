/* $Id$ */
/*
 Namespace.h : a class for local variables storage

 (c) 2007-2011 Michele Martone

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

	/*
	 * WARNING : TEMPORARILY, THIS FILE WILL HOST SOME CommandConsole's METHODS
	 * */

	int CommandConsole::setVariable(const fim::string& varname,int value)
	{
		/*
		 * an internal function to set a user variable
		 */
#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try
			{
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
			if( ns == 'w' )
#ifdef FIM_WINDOWS
			{
				//window variable
				if(window_)
					return window_->setVariable(id,value);
				else
					return 0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					return window_->current_viewportp()->setVariable(id,value);
				else
					return 0;
			}
			else
#endif
			if( ns == 'i' )
			{
				//image variable
				return
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->setVariable(id,value):
					0;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser_.setVariable(id,value);
			}
			else
			if( ns != 'g' )
			{
				//invalid namespace
				return 0;
			}
			}
			catch(FimException e){}
		}
#endif
		return variables_[varname].setInt(value);
	}

	float CommandConsole::setVariable(const fim::string& varname,float value)
	{
		/*
		 * an internal function to set a user variable
		 */
//		cout << "setVariable " << variables_[varname].setFloat(value) << "\n"; 
#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try
			{
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == 'w' )
			{
				//window variable
				if(window_)
					return window_->setVariable(id,value);
				else
					return 0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					return window_->current_viewportp()->setVariable(id,value);
				else
					return 0;
			}
			else
#endif
			if( ns == 'i' )
			{
				//image variable
				return
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->setVariable(id,value):
					0;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser_.setVariable(id,value);
			}
			else
			if( ns != 'g' )
			{
				//invalid namespace
				return 0;
			}
			}
			catch(FimException e){}
		}
#endif
		return variables_[varname].setFloat(value);
	}

	int CommandConsole::setVariable(const fim::string& varname,const char*value)
	{
		/*
		 * an internal function to set a user variable
		 */
		fim::string s(value);
#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			//a specific namespace was selected!
			try
			{
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == 'w' )
			{
				//window variable
				if(window_)
					return window_->setVariable(id,s.c_str());
				else
					return 0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					return window_->current_viewportp()->setVariable(id,s.c_str());
				else
					return 0;
			}
			else
#endif
			if( ns == 'i' )
			{
				//image variable
				return
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->setVariable(id,s.c_str()):
					0;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser_.setVariable(id,s.c_str());
			}
			else
			if( ns != 'g' )
			{
				//invalid namespace
				return 0;
			}
			}
			catch(FimException e){}
		}
#endif
		return (int)(variables_[varname].setString(s));
	}

	Var CommandConsole::setVariable(const fim::string varname,const Var&value)
	{
		/*
		 * an internal function to set a user variable
		 */
//		std::cout << "CCSV\n";
#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			//a specific namespace was selected!
			try
			{
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == 'w' )
			{
				//window variable
				if(window_)
					return window_->setVariable(id,value);
				else
					return 0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					return window_->current_viewportp()->setVariable(id,value);
				else
					return 0;
			}
			else
#endif
			if( ns == 'i' )
			{
				//image variable
				if(browser_.c_image())
				DBG(varname<<" : "<<value.getType()<<"\n");
				return
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->setVariable(id,value):
					Var(0);
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser_.setVariable(id,value);
			}
			else
			if( ns != 'g' )
			{
				//invalid namespace
				return 0;
			}
			}
			catch(FimException e){}
		}
#endif
		variables_[varname]=value;	/* FIXME : this does not work */
		variables_[varname].set(value);	/* FIXME : this works */
		//std::cout <<"CCGS["<<varname<<"]"<<variables_[varname].getString()<<"\n";
		//std::cout <<"CCGS["<<varname<<"]"<<value.getString()<<"\n";
		return value;;
	}


	int CommandConsole::getIntVariable(const fim::string &varname)const
	{
#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try
			{
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == 'w' )
			{
				//window variable
				if(window_)
					return window_->getIntVariable(id);
				else
					return 0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					return window_->current_viewportp()->getIntVariable(id);
				else
					return 0;
			}
			else
#endif
			if( ns == 'i' )
			{
				//image variable
				return
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->getIntVariable(id):
					0;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser_.getIntVariable(id);
			}
			else
			if( ns != 'g' )
			{
				//invalid namespace
				return 0;
			}
			}
			catch(FimException e){}
		}
#endif
		// this scope was selected
		/*
		 * the variable name supplied is used as a key to the variables hash
		 *
		 * FIXME : now the random stuff is done at interpreter level.
		 * AND IT SHOULD NOT BE USED INTERNALY BY FIM!
		 *
		 * BEWARE!
		 * */
		variables_t::const_iterator vi=variables_.find(varname);
		if(vi!=variables_.end()) return vi->second.getInt();
		else return 0;
		//return ((c_variables_t)(variables_))[varname].getInt();
	}

	float CommandConsole::getFloatVariable(const fim::string &varname)const
	{
		/*
		 * the variable name supplied is used as a key to the variables hash
		 * */
//		cout << "getVariable " << varname  << " : " << variables_[varname].getFloat()<< "\n";
//		cout << "getVariable " << varname  << ", type : " << variables_[varname].getType()<< "\n";
#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try{	
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == 'w' )
			{
				//window variable
				if(window_)
					return window_->getFloatVariable(id);
				else
					return FIM_CNS_EMPTY_FP_VAL;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					return window_->current_viewportp()->getFloatVariable(id);
				else
					return FIM_CNS_EMPTY_FP_VAL;
			}
			else
#endif
			if( ns == 'i' )
			{
				//image variable
				return
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->getFloatVariable(id):
					FIM_CNS_EMPTY_FP_VAL;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser_.getFloatVariable(id);
			}
			else
			if( ns != 'g' )
			{
				//invalid namespace
				return FIM_CNS_EMPTY_FP_VAL;
			}
			}
			catch(FimException e){}
		}
#endif
		variables_t::const_iterator vi=variables_.find(varname);
		if(vi!=variables_.end()) return vi->second.getFloat();
		else return FIM_CNS_EMPTY_FP_VAL;
//		return variables_[varname].getFloat();
	}

	fim::string CommandConsole::getStringVariable(const fim::string &varname)const
	{
		/*
		 * the variable name supplied is used as a key to the variables hash
		 * */
#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try
			{
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == 'w' )
			{
				//window variable
				if(window_)
					return window_->getStringVariable(id);
				else
					return "";
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					return window_->current_viewportp()->getStringVariable(id);
				else
					return "";
			}
			else
#endif
			if( ns == 'i' )
			{
				//image variable
				return
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->getStringVariable(id):
					"";
			}
			else
			if( ns == 'b' )
			{
				//browser variable
//				std::cout << "brbbbr\n";
				return browser_.getStringVariable(id);
			}
			else
			if( ns != 'g' )
			{
				//invalid namespace
				return "";
			}
			}
			catch(FimException e){}
		}
#endif
//		return variables_[varname].getString();
		variables_t::const_iterator vi=variables_.find(varname);
		if(vi!=variables_.end()) 
		{
			return vi->second.getString();
		}
		else return "";
	}

		int Namespace::setVariable(const fim::string& varname,int value)
		{
			return variables_[varname].setInt(value);
		}

		float Namespace::setVariable(const fim::string& varname,float value)
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

		int Namespace::setVariable(const fim::string& varname,const char*value)
		{
			/*
			 * an internal function to set a user variable
			 */
			fim::string s(value);
			return (int)(variables_[varname].setString(s));
		}
	
		int Namespace::getIntVariable(const fim::string &varname)const
		{
			// this scope was selected
			//return variables_[varname];
			variables_t::const_iterator vi=variables_.find(varname);
			if(vi!=variables_.end()) return vi->second.getInt();
			else return 0;
		}

		Var CommandConsole::getVariable(const fim::string &varname)const
		{
		/*
		 * the variable name supplied is used as a key to the variables hash
		 * */
#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try{	
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == 'w' )
			{
				//window variable
				if(window_)
					return window_->getVariable(id);
				else
					return Var();
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					return window_->current_viewportp()->getVariable(id);
				else
					return Var();
			}
			else
#endif
			if( ns == 'i' )
			{
				//image variable
				return
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->getVariable(id):
					Var();
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser_.getVariable(id);
			}
			else
			if( ns != 'g' )
			{
				//invalid namespace
				return Var();
			}
			}
			catch(FimException e){}
		}
#endif
		variables_t::const_iterator vi=variables_.find(varname);
		if(vi!=variables_.end()) return vi->second;
		else return Var();
		}

		Var Namespace::getVariable(const fim::string &varname)const
		{
			// this scope was selected
			variables_t::const_iterator vi=variables_.find(varname);
			if(vi!=variables_.end()) return vi->second;
			else return Var(0);
		}

		float Namespace::getFloatVariable(const fim::string &varname)const
		{
			/*
			 * the variable name supplied is used as a key to the variables hash
			 * */
//			cout << "getVariable " << varname  << " : " << variables_[varname].getFloat()<< "\n";
//			cout << "getVariable " << varname  << ", type : " << variables_[varname].getType()<< "\n";
			variables_t::const_iterator vi=variables_.find(varname);
			if(vi!=variables_.end()) return vi->second.getString();
			else return FIM_CNS_EMPTY_FP_VAL;
		}

		fim::string Namespace::getStringVariable(const fim::string &varname)const
		{
			/*
			 * the variable name supplied is used as a key to the variables hash
			 * */
//			std::cout << "NSGSV:"<<varname<<"\n";
			variables_t::const_iterator vi=variables_.find(varname);
			if(vi!=variables_.end())
			{
//				std::cout << "NSGSV:"<<vi->second.getString()<<"\n";
				return vi->second.getString();
			}
			else return "";
		}

	        float Namespace::setGlobalVariable(const fim::string& varname,float value)
		{
			return cc.setVariable(varname,value);
		}

		int Namespace::setGlobalVariable(const fim::string& varname,int value)
		{
			return cc.setVariable(varname,value);
		}

		int Namespace::setGlobalVariable(const fim::string& varname,const char*value)
		{
			return cc.setVariable(varname,value);
		}

		int Namespace::getGlobalIntVariable(const fim::string &varname)const
		{
			return cc.getIntVariable(varname);
		}

		float Namespace::getGlobalFloatVariable(const fim::string &varname)const
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
#else
			return "";
#endif
		}

		fim::string Namespace::get_variables_list()const
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
}

