/* $Id$ */
/*
 Arg.h : Argument class

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
#ifndef ARG_FBVI_H
#define ARG_FBVI_H
#include "fim.h"
namespace fim
{
class Arg
{
	public:
	fim::string val;
	Arg(const fim::string &s):val(s){}
	Arg(const Arg &arg):val(arg.val){}
	bool operator==(int i){int v=atoi(val.c_str());return v==i;}
	bool operator<=(int i){int v=atoi(val.c_str());return v<=i;}
	bool operator>=(int i){int v=atoi(val.c_str());return v>=i;}
	bool operator <(int i){int v=atoi(val.c_str());return v <i;}
	bool operator >(int i){int v=atoi(val.c_str());return v >i;}
};


}
#endif
