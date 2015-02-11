/* $LastChangedDate$ */
/*
 CommandConsole-var.h : CommandConsole variables store

 (c) 2013-2015 Michele Martone

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
#if FIM_WANT_LONG_INT
	fim_int CommandConsole::setVariable(const fim::string& varname,    int value)
	{
		return setVariable(varname,(fim_int) value);
	}
#endif /* FIM_WANT_LONG_INT */

	fim_int CommandConsole::setVariable(const fim::string& varname,fim_int value)
	{
		/*
		 * an internal function to set a user variable
		 */
		fim_int retval = 0;

#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try
			{
			//a specific namespace was selected!
			fim_char_t ns = varname[0];
			fim::string id=varname.c_str()+2;

			if( ns == FIM_SYM_NAMESPACE_WINDOW_CHAR )
#ifdef FIM_WINDOWS
			{
				//window variable
				if(window_)
					retval = window_->setVariable(id,value);
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_VIEWPORT_CHAR )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					retval = window_->current_viewportp()->setVariable(id,value);
				goto err;
			}
			else
#endif /* FIM_WINDOWS */
			if( ns == FIM_SYM_NAMESPACE_IMAGE_CHAR )
			{
				//image variable
				retval =	
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->setVariable(id,value):
					0;
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_BROWSER_CHAR )
			{
				//browser variable
				retval = browser_.setVariable(id,value);
				goto err;
			}
			else
			if( ns != FIM_SYM_NAMESPACE_GLOBAL_CHAR )
			{
				//invalid namespace
				goto err;
			}
			}
			catch(FimException e){}
		}
#endif /* FIM_NAMESPACES */
		retval = variables_[varname].setInt(value);
err:
		return retval;
	}

	fim_float_t CommandConsole::setVariable(const fim::string& varname,fim_float_t value)
	{
		/*
		 * an internal function to set a user variable
		 */
//		cout << "setVariable " << variables_[varname].setFloat(value) << "\n"; 
		fim_float_t retval = 0.0;

#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try
			{
			//a specific namespace was selected!
			fim_char_t ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == FIM_SYM_NAMESPACE_WINDOW_CHAR )
			{
				//window variable
				if(window_)
					retval = window_->setVariable(id,value);
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_VIEWPORT_CHAR )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					retval = window_->current_viewportp()->setVariable(id,value);
				goto err;
			}
			else
#endif /* FIM_WINDOWS */
			if( ns == FIM_SYM_NAMESPACE_IMAGE_CHAR )
			{
				//image variable
				retval = browser_.c_image()?
					( (Image*) (browser_.c_image()))->setVariable(id,value):
					0;
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_BROWSER_CHAR )
			{
				//browser variable
				retval = browser_.setVariable(id,value);
				goto err;
			}
			else
			if( ns != FIM_SYM_NAMESPACE_GLOBAL_CHAR )
			{
				//invalid namespace
				goto err;
			}
			}
			catch(FimException e){}
		}
#endif /* FIM_NAMESPACES */
		retval = variables_[varname].setFloat(value);
err:
		return retval;
	}

	fim_int CommandConsole::setVariable(const fim::string& varname,const fim_char_t*value)
	{
		/*
		 * an internal function to set a user variable
		 */
		fim::string s(value);
		fim_int retval = 0;

#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			//a specific namespace was selected!
			try
			{
			fim_char_t ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == FIM_SYM_NAMESPACE_WINDOW_CHAR )
			{
				//window variable
				if(window_)
					retval = window_->setVariable(id,s.c_str());
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_VIEWPORT_CHAR )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					retval = window_->current_viewportp()->setVariable(id,s.c_str());
				goto err;
			}
			else
#endif /* FIM_WINDOWS */
			if( ns == FIM_SYM_NAMESPACE_IMAGE_CHAR )
			{
				//image variable
				retval = 	
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->setVariable(id,s.c_str()):
					0;
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_BROWSER_CHAR )
			{
				//browser variable
				retval = browser_.setVariable(id,s.c_str());
				goto err;
			}
			else
			if( ns != FIM_SYM_NAMESPACE_GLOBAL_CHAR )
			{
				//invalid namespace
				goto err;
			}
			}
			catch(FimException e){}
		}
#endif /* FIM_WINDOWS */
		retval = (fim_int)(variables_[varname].setString(s));
err:
		return retval;
	}

	Var CommandConsole::setVariable(const fim::string varname,const Var&value)
	{
		/*
		 * an internal function to set a user variable
		 */
//		std::cout << "CCSV\n";
		Var retval = 0;

#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			//a specific namespace was selected!
			try
			{
			fim_char_t ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == FIM_SYM_NAMESPACE_WINDOW_CHAR )
			{
				//window variable
				if(window_)
					retval = window_->setVariable(id,value);
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_VIEWPORT_CHAR )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					retval = window_->current_viewportp()->setVariable(id,value);
				goto err;
			}
			else
#endif /* FIM_WINDOWS */
			if( ns == FIM_SYM_NAMESPACE_IMAGE_CHAR )
			{
				//image variable
				if(browser_.c_image())
				DBG(varname<<" : "<<value.getType()<<"\n");
				retval = 	
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->setVariable(id,value):
					Var(0);
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_BROWSER_CHAR )
			{
				//browser variable
				retval = browser_.setVariable(id,value);
				goto err;
			}
			else
			if( ns != FIM_SYM_NAMESPACE_GLOBAL_CHAR )
			{
				//invalid namespace
				goto err;
			}
			}
			catch(FimException e){}
		}
#endif /* FIM_NAMESPACES */
		variables_[varname]=value;	/* FIXME : this does not work */
		variables_[varname].set(value);	/* FIXME : this works */
		//std::cout <<"CCGS["<<varname<<"]"<<variables_[varname].getString()<<"\n";
		//std::cout <<"CCGS["<<varname<<"]"<<value.getString()<<"\n";
err:
		retval = value;
		return retval;
	}

	fim_int CommandConsole::getIntVariable(const fim::string &varname)const
	{
		fim_int retval = 0;

#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try
			{
			//a specific namespace was selected!
			fim_char_t ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == FIM_SYM_NAMESPACE_WINDOW_CHAR )
			{
				//window variable
				if(window_)
					retval = window_->getIntVariable(id);
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_VIEWPORT_CHAR )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					retval = window_->current_viewportp()->getIntVariable(id);
				goto err;
			}
			else
#endif /* FIM_WINDOWS */
			if( ns == FIM_SYM_NAMESPACE_IMAGE_CHAR )
			{
				//image variable
				retval = 	
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->getIntVariable(id):
					0;
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_BROWSER_CHAR )
			{
				//browser variable
				retval = browser_.getIntVariable(id);
				goto err;
			}
			else
			if( ns != FIM_SYM_NAMESPACE_GLOBAL_CHAR )
			{
				//invalid namespace
				goto err;
			}
			}
			catch(FimException e){}
		}
#endif /* FIM_NAMESPACES */
		// this scope was selected
		/*
		 * the variable name supplied is used as a key to the variables hash
		 *
		 * FIXME : now the random stuff is done at interpreter level.
		 * AND IT SHOULD NOT BE USED INTERNALY BY FIM!
		 *
		 * BEWARE!
		 * */
		{
			variables_t::const_iterator vi=variables_.find(varname);
			if(vi!=variables_.end())
				retval = vi->second.getInt();
		}
err:
		return retval;
		//return ((c_variables_t)(variables_))[varname].getInt();
	}

	fim_float_t CommandConsole::getFloatVariable(const fim::string &varname)const
	{
		/*
		 * the variable name supplied is used as a key to the variables hash
		 * */
//		cout << "getVariable " << varname  << " : " << variables_[varname].getFloat()<< "\n";
//		cout << "getVariable " << varname  << ", type : " << variables_[varname].getType()<< "\n";
		fim_float_t retval=FIM_CNS_EMPTY_FP_VAL;

#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try{	
			//a specific namespace was selected!
			fim_char_t ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == FIM_SYM_NAMESPACE_WINDOW_CHAR )
			{
				//window variable
				if(window_)
					retval = window_->getFloatVariable(id);
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_VIEWPORT_CHAR )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					retval = window_->current_viewportp()->getFloatVariable(id);
				goto err;
			}
			else
#endif /* FIM_WINDOWS */
			if( ns == FIM_SYM_NAMESPACE_IMAGE_CHAR )
			{
				//image variable
				retval = 	
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->getFloatVariable(id):
					FIM_CNS_EMPTY_FP_VAL;
				goto err;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_BROWSER_CHAR )
			{
				//browser variable
				retval = browser_.getFloatVariable(id);
				goto err;
			}
			else
			if( ns != FIM_SYM_NAMESPACE_GLOBAL_CHAR )
			{
				//invalid namespace
				retval = FIM_CNS_EMPTY_FP_VAL;
				goto err;
			}
			}
			catch(FimException e){}
		}
#endif /* FIM_NAMESPACES */
		{
			variables_t::const_iterator vi=variables_.find(varname);
			if(vi!=variables_.end())
				retval = vi->second.getFloat();
			else
			       	retval = FIM_CNS_EMPTY_FP_VAL;
		}
err:
		return retval;
//		return variables_[varname].getFloat();
	}

	fim::string CommandConsole::getStringVariable(const fim::string &varname)const
	{
		/*
		 * the variable name supplied is used as a key to the variables hash
		 * */
		fim::string retval = FIM_CNS_EMPTY_RESULT;

#ifdef FIM_NAMESPACES
		if( varname[1]==FIM_SYM_NAMESPACE_SEP )
		{
			try
			{
			//a specific namespace was selected!
			fim_char_t ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == FIM_SYM_NAMESPACE_WINDOW_CHAR )
			{
				//window variable
				if(window_)
					retval = window_->getStringVariable(id);
				goto ret;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_VIEWPORT_CHAR )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					retval = window_->current_viewportp()->getStringVariable(id);
				goto ret;
			}
			else
#endif /* FIM_WINDOWS */
			if( ns == FIM_SYM_NAMESPACE_IMAGE_CHAR )
			{
				//image variable
				retval = 	
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->getStringVariable(id):
					FIM_CNS_EMPTY_RESULT;
				goto ret;
			}
			else
			if( ns == FIM_SYM_NAMESPACE_BROWSER_CHAR )
			{
				//browser variable
//				std::cout << "brbbbr\n";
				retval = browser_.getStringVariable(id);
				goto ret;
			}
			else
			if( ns != FIM_SYM_NAMESPACE_GLOBAL_CHAR )
			{
				//invalid namespace
				retval = FIM_CNS_EMPTY_RESULT;
				goto ret;
			}
			}
			catch(FimException e){}
		}
#endif /* FIM_NAMESPACES */
//		return variables_[varname].getString();
		{
			variables_t::const_iterator vi=variables_.find(varname);
			if(vi!=variables_.end()) 
			retval = vi->second.getString();
		}
ret:
		return retval;
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
			fim_char_t ns = varname[0];
			fim::string id=varname.c_str()+2;
#ifdef FIM_WINDOWS
			if( ns == FIM_SYM_NAMESPACE_WINDOW_CHAR )
			{
				//window variable
				if(window_)
					return window_->getVariable(id);
				else
					return Var();
			}
			else
			if( ns == FIM_SYM_NAMESPACE_VIEWPORT_CHAR )
			{
				//viewport variable
				if(window_ && window_->current_viewportp())
					return window_->current_viewportp()->getVariable(id);
				else
					return Var();
			}
			else
#endif /* FIM_WINDOWS */
			if( ns == FIM_SYM_NAMESPACE_IMAGE_CHAR )
			{
				//image variable
				return
					browser_.c_image()?
					( (Image*) (browser_.c_image()))->getVariable(id):
					Var();
			}
			else
			if( ns == FIM_SYM_NAMESPACE_BROWSER_CHAR )
			{
				//browser variable
				return browser_.getVariable(id);
			}
			else
			if( ns != FIM_SYM_NAMESPACE_GLOBAL_CHAR )
			{
				//invalid namespace
				return Var();
			}
			}
			catch(FimException e){}
		}
#endif /* FIM_NAMESPACES */
		variables_t::const_iterator vi=variables_.find(varname);

		if(vi!=variables_.end())
			return vi->second;
		else
		       	return Var();
		}

}
