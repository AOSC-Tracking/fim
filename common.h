#ifndef COMMON_H
#define COMMON_H

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
};

//live from src/loader.h:
enum ida_extype {
    EXTRA_COMMENT = 1,
    EXTRA_EXIF    = 2,
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








#endif
