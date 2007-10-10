/* $Id$ */
/*
 Namespace.h : a class for local variables storage

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
		if( varname[1]==':' )
		{
			try
			{
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
			if( ns == 'w' )
			{
				//window variable
				if(window)
					return window->setVariable(id,value);
				else
					return 0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window && window->current_viewportp())
					return window->current_viewportp()->setVariable(id,value);
				else
					return 0;
			}
			else
			if( ns == 'i' )
			{
				//image variable
				return
					browser.c_image()?
					( (Image*) (browser.c_image()))->setVariable(id,value):
					0;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser.setVariable(id,value);
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
		return variables[varname].setInt(value);
	}

	float CommandConsole::setVariable(const fim::string& varname,float value)
	{
		/*
		 * an internal function to set a user variable
		 */
//		cout << "setVariable " << variables[varname].setFloat(value) << "\n"; 
#ifdef FIM_NAMESPACES
		if( varname[1]==':' )
		{
			try
			{
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
			if( ns == 'w' )
			{
				//window variable
				if(window)
					return window->setVariable(id,value);
				else
					return 0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window && window->current_viewportp())
					return window->current_viewportp()->setVariable(id,value);
				else
					return 0;
			}
			else
			if( ns == 'i' )
			{
				//image variable
				return
					browser.c_image()?
					( (Image*) (browser.c_image()))->setVariable(id,value):
					0;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser.setVariable(id,value);
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
		return variables[varname].setFloat(value);
	}

	int CommandConsole::setVariable(const fim::string& varname,const char*value)
	{
		/*
		 * an internal function to set a user variable
		 */
		fim::string s(value);
#ifdef FIM_NAMESPACES
		if( varname[1]==':' )
		{
			//a specific namespace was selected!
			try
			{
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
			if( ns == 'w' )
			{
				//window variable
				if(window)
					return window->setVariable(id,s.c_str());
				else
					return 0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window && window->current_viewportp())
					return window->current_viewportp()->setVariable(id,s.c_str());
				else
					return 0;
			}
			else
			if( ns == 'i' )
			{
				//image variable
				return
					browser.c_image()?
					( (Image*) (browser.c_image()))->setVariable(id,s.c_str()):
					0;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser.setVariable(id,s.c_str());
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
		return (int)(variables[varname].setString(s));
	}


	int CommandConsole::getIntVariable(const fim::string &varname)
	{
#ifdef FIM_NAMESPACES
		if( varname[1]==':' )
		{
			try
			{
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
			if( ns == 'w' )
			{
				//window variable
				if(window)
					return window->getIntVariable(id);
				else
					return 0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window && window->current_viewportp())
					return window->current_viewportp()->getIntVariable(id);
				else
					return 0;
			}
			else
			if( ns == 'i' )
			{
				//image variable
				return
					browser.c_image()?
					( (Image*) (browser.c_image()))->getIntVariable(id):
					0;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser.getIntVariable(id);
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
		return variables[varname].getInt();
	}

	float CommandConsole::getFloatVariable(const fim::string &varname)
	{
		/*
		 * the variable name supplied is used as a key to the variables hash
		 * */
//		cout << "getVariable " << varname  << " : " << variables[varname].getFloat()<< "\n";
//		cout << "getVariable " << varname  << ", type : " << variables[varname].getType()<< "\n";
#ifdef FIM_NAMESPACES
		if( varname[1]==':' )
		{
			try{	
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
			if( ns == 'w' )
			{
				//window variable
				if(window)
					return window->getFloatVariable(id);
				else
					return 0.0;
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window && window->current_viewportp())
					return window->current_viewportp()->getFloatVariable(id);
				else
					return 0.0;
			}
			else
			if( ns == 'i' )
			{
				//image variable
				return
					browser.c_image()?
					( (Image*) (browser.c_image()))->getFloatVariable(id):
					0.0;
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser.getFloatVariable(id);
			}
			else
			if( ns != 'g' )
			{
				//invalid namespace
				return 0.0;
			}
			}
			catch(FimException e){}
		}
#endif
		return variables[varname].getFloat();
	}

	fim::string CommandConsole::getStringVariable(const fim::string &varname)
	{
		/*
		 * the variable name supplied is used as a key to the variables hash
		 * */
#ifdef FIM_NAMESPACES
		if( varname[1]==':' )
		{
			try
			{
			//a specific namespace was selected!
			char ns = varname[0];
			fim::string id=varname.c_str()+2;
			if( ns == 'w' )
			{
				//window variable
				if(window)
					return window->getStringVariable(id);
				else
					return "";
			}
			else
			if( ns == 'v' )
			{
				//viewport variable
				if(window && window->current_viewportp())
					return window->current_viewportp()->getStringVariable(id);
				else
					return "";
			}
			else
			if( ns == 'i' )
			{
				//image variable
				return
					browser.c_image()?
					( (Image*) (browser.c_image()))->getStringVariable(id):
					"";
			}
			else
			if( ns == 'b' )
			{
				//browser variable
				return browser.getStringVariable(id);
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
		return variables[varname].getString();
	}
}

