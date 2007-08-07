/* $Id$ */
/*
 Var.h : Var class header file

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

/*
 * This class is horrible. Maybe it has come the time of taking advantage of using really C++
 * and using classes hierarchies.
 */
#ifndef VAR_FBVI_H
#define VAR_FBVI_H

#include "fim.h"
#include "string.h"
namespace fim
{
class Var
{
	union{float f;
	int i;
	};
	fim::string s;
	int type;
	public:
	Var(const Var &v)
	{
		this->type=v.type;
		if(type=='i')this->i=v.i;
		if(type=='f')this->f=v.f;
	}
	Var(const char*s="0",int type_='i')
	{
		type=type_;
		if(type=='i')i=atoi(s);
		else if(type=='f')f=atof(s);
		else if(type=='s')this->s=s;
		else i=0;
	}
/*	void operator= (int   i){if(type=='i')this->i=i;}
	void operator= (float f){if(type=='f')this->f=f;}*/
	void operator= (int   i){type='i';this->i=i;}
	void operator= (float f){setFloat(f);}
	void operator= (fim::string &s){setString(s);}
	float setFloat(float f){type='f';return this->f=f;}
	int   setInt(int i){type='i';return this->i=i;}
	fim::string setString(fim::string &s){type='s';this->s=s;return this->s;}
	int getType()const{return type;}
	int getInt()const{return(type=='i')?i:
		(type=='f'?((int)(f)):
		 (type=='s'?(atoi(s.c_str())):0)
		 )
		;}
	float getFloat()const{return(type=='f')?f:
		(type=='i'?
		 	((int)f):
			((type=='s')?atof(s.c_str()):0.0f)
			)
			;}
	fim::string getString()
	{
		char buf[16];
		if(type=='s')return this->s;
		else
		{
			if(type=='i')sprintf(buf,"%d",i);
			else if(type=='f')sprintf(buf,"%f",f);
			return buf;
		}
		
	}
	operator int()const{return getInt();}
	operator float()const{return getFloat();}
	bool operator==(const Var &v)const
	{
		return (type==v.getType()) && (i==v.getInt());
	}
};
}


#endif
