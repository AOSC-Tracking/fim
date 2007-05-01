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
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
*/
#include "fim.h"

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


