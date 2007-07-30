/* $Id$ */
#ifndef STRING_FBVI_H
#define STRING_FBVI_H
/*
 string.h : Fim's own string implementatino header file

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

/*
 * if defined, gives problems with STL templates ..
 */
//#define _FIM_DYNAMIC_STRING 1

namespace fim
{
#define fim_free(x) {free(x);}
//#define fim_free(x) {std::cout<<"freeing "<<(int*)x<<"\n";free(x);x=NULL;std::cout<<"freeed!\n";}
#define fim_calloc(x) calloc((x),1)
#define fim_malloc(x) malloc(x)
#define fim_realloc(x,n) realloc((x),(n))
#define fim_empty_string(s) (!(s) || !(*(s)))

#define ferror(s) {/*fatal error*/fprintf(stderr,"%s,%d:%s(please submit this error as a bug!)\n",__FILE__,__LINE__,s);cc.quit(-1);}
	/*
	 *	Allocation and duplication of a single string
	 */
	static char * fim_dupstr (const char* s);

	class string{
	/*
	 * this is a declaration of my love to the STL.. (i wrote this dumb code 
	 * in dumb 10 minutes after spending dumb hours messing with the original
	 * STL string template .. ) ( am i dumb ? ghghhh.. probably .. )
	 *
	 * FIX ME : all of this should be made dynamic, but with the right semantics.
	 */
        static const int TOKSIZE=128*8*4*2;	//max len.NUL included
#ifdef _FIM_DYNAMIC_STRING
	char*s;		/* the string : can be NULL */
	int len;	/* the allocated amount */
#else
	char s[TOKSIZE];
#endif
	public :
	void _string_init();

	int reallocate(int l);

	int reset(int l);

	bool isempty()const;

	~string();
	string();
	string(const string& s);
	string(const char *str);
	string(const int &i);
	string(const unsigned int &i);
	const char*c_str()const;
	bool operator==(const string& s)const;
	bool operator==(const char *  s)const;
	bool operator!=(const string& s)const;
	bool operator<=(const string& s)const;
	bool operator>=(const string& s)const;
	bool operator <(const string& s)const;
	bool operator >(const string& s)const;
	bool operator >(const char *s)const;
	bool operator <(const char *s)const;

	string& operator =(const string& s);
	string operator+=(const string& s);
	string operator+(const string& s)const;
	int  reinit(const int n)const;
	int  length()const;
	static int  max_string(){return TOKSIZE-1;}
	int  size()const;
	int  find(const string&str)const;
	int  assign(const string&str);
	int  assign(const char*str);
	int  find(const char*ss)const;
 	std::ostream& print(std::ostream &os)const;
//	int operator=(int &i,const string& s){i=-1;return i;}
	operator int()const{return atoi(s);}
	operator float()const{return atof(s);}
	};

}


#endif
