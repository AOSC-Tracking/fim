/* $Id$ */
/*
 common.h : Miscellaneous stuff header file

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




using namespace fim;



fim::string slurp_file(fim::string filename);
void append_to_file(fim::string filename, fim::string lines);





#endif
