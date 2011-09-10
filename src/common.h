/* $LastChangedDate$ */
/*
 common.h : Miscellaneous stuff header file

 (c) 2007-2011 Michele Martone

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
#ifndef FIM_COMMON_H
#define FIM_COMMON_H
#undef yyFlexLexer
#include <FlexLexer.h>
#include "fim_types.h"
extern int fim_pipedesc[2];

extern FlexLexer *lexer;
//#define YYLEX lexer->yylex()
namespace fim
{
	class CommandConsole;
	class string;
}

int fim_isspace(int c);
int fim_isquote(int c);

fim::string fim_dirname(const fim::string & arg);
fim::string fim_shell_arg_escape(const fim::string & arg);
void fim_perror(const fim_char_t *s);
size_t fim_strlen(const fim_char_t *str);
void trec(fim_char_t *str,const fim_char_t *f,const fim_char_t*t);
void trhex(fim_char_t *str);
void chomp(fim_char_t *s);
void sanitize_string_from_nongraph(fim_char_t *s, int c=0);
void sanitize_string_from_nongraph_except_newline(fim_char_t *s, int c=0);

using namespace fim;

int int2msbf(int in);
int int2lsbf(int in);
fim::string slurp_file(fim::string filename);
fim_char_t* slurp_binary_fd(int fd,int *rs);
fim_byte_t* slurp_binary_FD(FILE* fd, size_t  *rs);
void append_to_file(fim::string filename, fim::string lines);

fim_char_t * dupstr (const fim_char_t* s);
fim_char_t * dupnstr (float n, const fim_char_t c='\0');
fim_char_t * dupnstr (const fim_char_t c1, double n, const fim_char_t c2='\0');
fim_char_t * dupnstr (int n);
fim_char_t * dupsqstr (const fim_char_t* s);
int fim_rand();

bool regexp_match(const fim_char_t*s, const fim_char_t*r, int ignorecase=1, int ignorenewlines=0);

int strchr_count(const fim_char_t*s, int c);
int lines_count(const fim_char_t*s, int cols);
int newlines_count(const fim_char_t*s);
const fim_char_t* next_row(const fim_char_t*s, int cols);
int fim_common_test();

double getmilliseconds();
const fim_char_t * fim_getenv(const fim_char_t * name);
FILE * fim_fread_tmpfile(FILE * fp);
double fim_atof(const fim_char_t *nptr);
ssize_t fim_getline(fim_char_t **lineptr, size_t *n, FILE *stream);

bool is_dir(const fim::string nf);
bool is_file(const fim::string nf);

/* exceptions */
typedef int FimException;
#define FIM_E_NO_IMAGE 1
#define FIM_E_NO_VIEWPORT 2
#define FIM_E_WINDOW_ERROR 3
#define FIM_E_TRAGIC -1	/* no hope */
#define FIM_E_NO_MEM 4	/* also a return code */
/* ... */



#endif
