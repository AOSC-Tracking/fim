/* $Id$ */
/*
 common.cpp : Miscellaneous stuff..

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

// including fim.h poses incompatibilities with <fstream>
//#include "fim.h"

#include <iostream>
#include "string.h"
#include <string>
#include <fstream>

void trec(char *str,const char *f,const char*t)
{
	/*	this function translates escaped characters at index i in 
	 *	f into the characters at index i in t.
	 *
	 *	order is not important for the final effect.
	 * 
	 *	this function could be optimized.
	 */
	if(!str || !f || !t || strlen(f)-strlen(t))return;
	char*_p=str;
	const char *fp;
	const char *tp;
	while(*_p && _p[1])//redundant ?
	{
		fp=f;
		tp=t;
		while(*fp)
		{
			if( *_p == '\\' && *(_p+1) == *fp )
			{
				*_p = *tp;//translation	
				++_p;  //new focus
				char*pp;
				pp=_p+1;
				while(*pp){pp[-1]=*pp;++pp;}
				pp[-1]='\0';
				//if(*_p=='\\')++_p;//we want a single pass
				if(*_p)++_p;//we want a single pass
				if(!*_p)return;
				continue;
			}
			++fp;++tp;
		}
		++_p;
	} 


}

	fim::string slurp_file(fim::string filename)
	{
		std::string file;
		std::ifstream fs;
		fs.open(filename.c_str(),std::ios::binary);
		if(fs.is_open())
		{
			std::string tmp;
			/* FIXME : this is not efficient */
			while(!fs.eof())
			{
				getline(fs,tmp);
				tmp+="\n" ;
				file+=tmp;
			}
				printf("%s\n",file.c_str());
		}
		fs.close();
		return fim::string(file.c_str());
	}

	void append_to_file(fim::string filename, fim::string lines)
	{
		std::ofstream fs;
		fs.open(filename.c_str(),std::ios::app|std::ios::binary);
		if(fs.is_open())
		{
			fs << "\""<< lines.c_str()<<"\"" ;
		}
		fs.close();
		sync();
	}


