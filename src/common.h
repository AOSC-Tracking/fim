/* $Id$ */
/*
 common.h : Miscellaneous stuff header file

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
//#include "lex.h"
//#include "src/loader.h"
//#include "src/list.h"
extern int pipedesc[2];

extern FlexLexer *lexer;
//#define YYLEX lexer->yylex()
namespace fim
{
	class CommandConsole;
	class string;
	//extern fim::CommandConsole cc;
}

//live from src/loader.h:
enum ida_extype {
    EXTRA_COMMENT = 1,
    EXTRA_EXIF    = 2
};

struct ida_extra {
    enum ida_extype   type;
    unsigned char     *data;
    unsigned int      size;
    struct ida_extra  *next;
};

/* image data and metadata */
struct ida_image_info {
    unsigned int      width;
    unsigned int      height;
    unsigned int      dpi;
    unsigned int      npages;
    struct ida_extra  *extra;

    int               thumbnail;
    unsigned int      real_width;
    unsigned int      real_height;
};

struct ida_image {
    struct ida_image_info  i;
    unsigned char          *data;
};





//void trec(char *str,char ec);
void trec(char *str,const char *f,const char*t);




using namespace fim;



fim::string slurp_file(fim::string filename);
void append_to_file(fim::string filename, fim::string lines);





#endif
