/* $LastChangedDate$ */
#ifndef FIM_STRING_H
#define FIM_STRING_H
/*
 string.h : Fim's string type

 (c) 2007-2017 Michele Martone

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
 * the rest of the program waits still for a proper adaptation of dynamic strings.
 */
#define FIM_CHARS_FOR_INT 32 /* should fit a pointer address printout */

namespace fim
{
	class string:public std::string
	{
		public:
		string();
		~string(){}

		/*
			 if not, exception:
			 terminate called after throwing an instance of 'std::logic_error'
			 what():  basic_string::_S_construct FIM_NULL not valid
		*/
		string(const std::string&rhs):std::string(rhs){}
		string(const fim_char_t*s):std::string(s?s:""){}

		string(fim_char_t c);
#if FIM_WANT_LONG_INT
		string(int i);
#endif /* FIM_WANT_LONG_INT */
		string(fim_int i);
		string(float i);
		string(int * i);
		string(size_t i);

/*
 		the following two operators are very nice to use but pose unexpected problems.		
*/
 		operator fim_int  (void)const;
#if FIM_WANT_LONG_INT
 		operator int  (void)const;
#endif /* FIM_WANT_LONG_INT */
		operator float(void)const;
		operator const char*(void)const{return c_str();}
		operator bool (void)const{return c_str()!=NULL;}

		string operator+(const string rhs)const;
		/* copy constructor */
		string(const string& rhs);
		bool re_match(const fim_char_t*r)const;
		void substitute(const fim_char_t*r, const fim_char_t* s, int flags=0);
		fim::string line(int ln)const;
		size_t lines(void)const;
		int find_re(const fim_char_t*r,int *mbuf=FIM_NULL)const;
	}; /* fim::string */
}
#endif /* FIM_STRING_H */
