/* $Id$ */
/*
 fim_stream.h : Textual output facility

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
#ifndef FBVI_STREAM_FBVI_H
#define FBVI_STREAM_FBVI_H
#include "fim.h"
namespace fim
{


	class fim_stream
//	:public std::ostream
	{/*
std::basic_ostream<char, std::char_traits<char> >::basic_ostream(const <anonymous>**)'
		      are: std::basic_ostream<char, std::char_traits<char> >::basic_ostream(const
				         std::basic_ostream<char, std::char_traits<char> >&)
		      /usr/lib/gcc-lib/i686-pc-linux-gnu/3.3.6/include/g++-v3/ostream:106: error:
		                     std::basic_ostream<_CharT,
	      _Traits>::basic_ostream(std::basic_streambuf<_CharT, _Traits>*) [with _CharT
		         = char, _Traits = std::char_traits<char>]
*/
//		std::ostream os;
		public:
	//	basic_ostream()
		fim_stream(){}
		fim_stream& operator<<(const  char* s);
		fim_stream& operator<<(const unsigned char* s)
		{
			*this<<(const char*)s;
			return *this;
		}
		fim_stream& operator<<(const  fim::string&s)
		{
			*this<<(const  char*)(s.c_str());
			return *this;
		}
		fim_stream& operator<<(float f)
		{
			char s[32];sprintf(s,"%f",f);
			*this<<(const char*)s;
			return *this;
		}
		fim_stream& operator<<(int i)
		{
			char s[32];sprintf(s,"%d",i);
			*this<<s;
			return *this;
		}
	};
}
#endif
