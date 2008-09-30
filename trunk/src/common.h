/* $Id$ */
/*
 common.h : Miscellaneous stuff header file

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
#ifndef COMMON_H
#define COMMON_H
#undef yyFlexLexer
#include <FlexLexer.h>
#include <linux/fb.h>
extern int pipedesc[2];

extern FlexLexer *lexer;
//#define YYLEX lexer->yylex()
namespace fim
{
	class CommandConsole;
	class string;
}


void trec(char *str,const char *f,const char*t);
void chomp(char *s);
void sanitize_string_from_nongraph(char *s, int c=0);
void sanitize_string_from_nongraph_except_newline(char *s, int c=0);

using namespace fim;

int int2msbf(int in);
int int2lsbf(int in);
fim::string slurp_file(fim::string filename);
char* slurp_binary_fd(int fd,int *rs);
void append_to_file(fim::string filename, fim::string lines);

char * dupstr (const char* s);
char * dupnstr (double n);
char * dupnstr (int n);
int fim_rand();

bool regexp_match(const char*s, const char*r, int ignorecase=1, int ignorenewlines=0);

int strchr_count(const char*s, int c);
int lines_count(const char*s, int cols);
int newlines_count(const char*s);
const char* next_row(const char*s, int cols);
int fim_common_test();
/* exceptions */
typedef int FimException;
#define FIM_E_NO_IMAGE 1
#define FIM_E_NO_VIEWPORT 2
#define FIM_E_WINDOW_ERROR 3
#define FIM_E_TRAGIC -1	/* no hope */
#define FIM_E_NO_MEM 4	/* also a return code */
/* ... */



#endif
