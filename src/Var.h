/* $Id$ */
/*
 Var.h : Var class header file

 (c) 2007-2008 Michele Martone

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
 * This class is horrible. 
 */
#ifndef VAR_FIM_H
#define VAR_FIM_H

#include "fim.h"
#include "string.h"
namespace fim
{
class Var
{
	union
	{
		float f;
		int i;
	};
	fim::string s;
	int type;
	public:
	Var(const Var &v)
	:type(0),i(0),s(fim::string())
	{
		this->type=v.type;
		if(type=='i')this->i=v.i;
		if(type=='f')this->f=v.f;
		if(type=='s')this->s=v.s;
	}

	Var(float v)
	:type(0),i(0),s(fim::string())
	{
		type='f';
		f=v;
	}

	Var(int v)
	:type(0),i(0),s(fim::string())
	{
		type='i';
		i=v;
	}

	Var()
	:type(0),i(0),s(fim::string())
	{
		type='i';
		const char *s="0";
		if(type=='i')i=atoi(s);
		else if(type=='f')f=atof(s);
		else if(type=='s')this->s=s;
		else i=0;
	}

	Var(const fim::string s)
	:type(0),i(0),s(fim::string())
	{
		type='s';
		this->s=s.c_str();
	}

	Var(const char*s)
	:type(0),i(0),s(fim::string())
	{
		type='s';
		this->s=s;
	}
/*
	Var(const char*s="0",int type_='i')
	{
		type=type_;
		if(type=='i')i=atoi(s);
		else if(type=='f')f=atof(s);
		else if(type=='s')this->s=s;
		else i=0;
	}
*/
/*	void operator= (int   i){if(type=='i')this->i=i;}
	void operator= (float f){if(type=='f')this->f=f;}*/
#if 0
	void operator= (int   i){type='i';this->i=i;}
	void operator= (float f){setFloat(f);}
	void operator= (fim::string &s){setString(s);}
#else
	const Var& operator= (int   i){type='i';this->i=i;return *this;}
	const Var& operator= (float f){setFloat(f);return *this;}
	const Var& operator= (fim::string &s){setString(s);return *this;}
#endif
	float setFloat(float f){type='f';return this->f=f;}
	int   setInt(int i){type='i';return this->i=i;}
	fim::string setString(fim::string &s){type='s';this->s=s;return this->s;}
	int getType()const{return type;}
	int getInt()const{return(type=='i')?i:
		(type=='f'?((int)(f)):
		 (type=='s'?(atoi(s.c_str())):0)
		 )
		;}

	float getFloat()const{
	
	return(type=='f')?f:
		(type=='i'?
		 	((float)i):
			((type=='s')?atof(s.c_str()):0.0f)
			)
			;}

	fim::string getString()const
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
	operator string()const{return getString();}

	/*
	 * These should be refined some day :)
	 * */
	Var  operator<=(const Var &v)const { return getFloat()<=v.getFloat(); }
	Var  operator>=(const Var &v)const { return getFloat()>=v.getFloat(); }
	Var  operator< (const Var &v)const { return getFloat()< v.getFloat(); }
	Var  operator> (const Var &v)const { return getFloat()> v.getFloat(); }
	#define _both(t) (((getType()==t) && (v.getType()==t)))
	#define _some_string() (getType()=='s' || v.getType()=='s')
	#define _numeric() (!_some_string())
	#define _p_t(op) std::cout<<op<<"("<<(char)getType()<<","<<(char)v.getType()<<")\n";
	Var operator!=(const Var &v)const {
		//_p_t("!=")
		if(_both('i'))return getInt  () !=v.getInt  (); 
		if(_both('f'))return getFloat() !=v.getFloat();
		if(_both('s'))
		{
			return getString()!=v.getString(); 
		}
		return getFloat()!=v.getFloat();
	}
	Var operator==(const Var &v)const { return 1-(int)(*this != v); }
	Var operator/ (const Var &v)const
	{
		if(_both('i')) return getInt()/(v.getInt()!=0?v.getInt():1); 
		return getFloat()/(v.getFloat()!=0.0?v.getFloat():1); 
	}
	Var operator* (const Var &v)const
	{
		if(_both('i'))return getInt  ()*v.getInt  (); 
		if(_both('f'))return getFloat()*v.getFloat();
		if(_both('s'))return getFloat()*v.getFloat();
		return getFloat()*v.getFloat(); 
	}
	Var operator+ (const Var &v)const
	{
		if(_both('i'))return getInt  ()+v.getInt  (); 
		if(_both('f'))return getFloat()+v.getFloat();
		if(_both('s'))return getString()+v.getString();//yes...
		return getFloat()+v.getFloat(); 
	}
	Var operator- (const Var &v)const
	{
		if(_both('i'))return getInt  ()-v.getInt  (); 
		if(_both('f'))return getFloat()-v.getFloat();
		if(_both('s'))return getFloat()-v.getFloat();//yes...
		return getFloat()-v.getFloat(); 
	}
	Var operator- ()const {
	if(getType()=='i')return - getInt  (); 
	if(getType()=='f')return - getFloat(); 
	if(getType()=='s')return - getFloat(); 
	}
	Var operator% (const Var &v)const { return getInt()%v.getInt(); }
//	Var operator, (const Var &v)const { return (getString()+v.getString()); }
	Var operator&&(const Var &v)const { return getInt()&&v.getInt(); }
	Var operator||(const Var &v)const { return getInt()||v.getInt(); }
	Var re_match(const Var &v)const { return getString().re_match(v.getString().c_str()); }
	#undef _p_t
	#undef _numeric
	#undef _some_string
	#undef _both
/*	Var operator==(const Var &v)const
	{
		return (type==v.getType()) && (i==v.getInt());
	}*/
};
}


#endif
