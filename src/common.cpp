/* $Id$ */
/*
 common.cpp : Miscellaneous stuff..

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

// including fim.h poses incompatibilities with <fstream>
//#include "fim.h"

#include <iostream>
#include "fim.h"
#include "string.h"
#include "common.h"
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
			//	printf("%s\n",file.c_str());
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

/*
 * Turns newline characters in NULs.
 * Does stop on the first NUL encountered.
 */
void chomp(char *s)
{
	for(;*s;++s)if(*s=='\n')*s='\0';
}

/*
 * cleans the input string terminating it when some non printable character is encountered
 * */
void sanitize_string_from_nongraph(char *s, int c=0)
{	
	int n=c;
	if(s)
	while(*s && (c--||!n))if(!isgraph(*s)||*s=='\n'){*s=' ';++s;}else ++s;
	return;
}

/*
 *	Allocation of a small string for storing the 
 *	representation of a double.
 */
char * dupnstr (double n)
{
	//allocation of a single string
	char *r = (char*) malloc (16);
	if(!r){/*assert(r);*/throw FIM_E_NO_MEM;}
	sprintf(r,"%f",n);
	return (r);
}

/*
 *	Allocation of a small string for storing the *	representation of an integer.
 */
char * dupnstr (int n)
{
	//allocation of a single string
	char *r = (char*) malloc (16);
	if(!r){/*assert(r);*/throw FIM_E_NO_MEM;}
	sprintf(r,"%d",n);
	return (r);
}

/*
 *	Allocation and duplication of a single string
 */
char * dupstr (const char* s)
{
	char *r = (char*) malloc (strlen (s) + 1);
	if(!r){/*assert(r);*/throw FIM_E_NO_MEM;}
	strcpy (r, s);
	return (r);
}

static int pick_word(char *f, unsigned int *w)
{
	int fd = open(f,O_RDONLY);
	if(fd==-1) return -1;
	if(read(fd,w,4)==4)return 0;
	return -1;
}


/*
 * Will be improved, if needed.
 * */
int fim_rand()
{
	/*
	 * Please don't use Fim for cryptographical purposes ;)
	 * */
	unsigned int w;
	if(pick_word("/dev/urandom",&w)==0) return (w%RAND_MAX);
	
	srand(clock()); return rand();

}

	bool regexp_match(const char*s, const char*r, int ignorecase, int ignorenewlines)
	{
		/*
		 *	given a string s, and a Posix regular expression r, this
		 *	method returns true if there is match. false otherwise.
		 */
		regex_t regex;		//should be static!!!
		const int nmatch=1;	// we are satisfied with the first match, aren't we ?
		regmatch_t pmatch[nmatch];

		/*
		 * we allow for the default match, in case of null regexp
		 */
		if(!r || !strlen(r))return true;

		/* fixup code for a mysterious bug
		 */
		if(*r=='*')return false;

		fim::string aux;
		if(ignorenewlines)
		{
			aux=s;
		}

		//if(regcomp(&regex,"^ \\+$", 0 | REG_EXTENDED | REG_ICASE )==-1)
		if(regcomp(&regex,r, 0 | REG_EXTENDED | (ignorecase==0?0:REG_ICASE) )==-1)
		{
			/* error calling regcomp (invalid regexp?)! (should we warn the user ?) */
			//cout << "error calling regcomp (invalid regexp?)!" << "\n";
			return false;
		}
		else
		{
//			cout << "done calling regcomp!" << "\n";
		}
		//if(regexec(&regex,s+0,nmatch,pmatch,0)==0)
		if(regexec(&regex,s+0,nmatch,pmatch,0)!=REG_NOMATCH)
		{
//			cout << "'"<< s << "' matches with '" << r << "'\n";
/*			cout << "match : " << "\n";
			cout << "\"";
			for(int m=pmatch[0].rm_so;m<pmatch[0].rm_eo;++m)
				cout << s[0+m];
			cout << "\"\n";*/
			regfree(&regex);
			return true;
		}
		else
		{
			/*	no match	*/
		};
		regfree(&regex);
		return false;
		return true;
	}

