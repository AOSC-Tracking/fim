/* $Id$ */
/*
 FontServer.h : Font Server code from fbi, adapted for fim.

 (c) 2008 Michele Martone
 (c) 1998-2006 Gerd Knorr <kraxel@bytesex.org>

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
 * This file comes from fbi, and will undergo severe reorganization.
 * */


#ifndef FONT_SERVER_FIM_H
#define FONT_SERVER_FIM_H

//# include "FramebufferDevice.h"

# include <errno.h>
# include <unistd.h> //for NULL
# include <stdio.h> //for stderr..
# include <stdlib.h> //for exit..
# include <string.h> //for strlen..
namespace fim
{


#ifndef X_DISPLAY_MISSING
# include <FSlib.h>

struct fs_font {
    Font               font;
    FSXFontInfoHeader  fontHeader;
    FSPropInfo         propInfo;
    FSPropOffset       *propOffsets;
    unsigned char      *propData;

    FSXCharInfo        *extents;
    FSOffset           *offsets;
    unsigned char      *glyphs;

    int                maxenc,width,height;
    FSXCharInfo        **eindex;
    unsigned char      **gindex;
};

#else

typedef struct _FSXCharInfo {
    short       left;
    short       right;
    short       width;
    short       ascent;
    short       descent;
    /*unsigned short      attributes;*/
} FSXCharInfo;

typedef struct _FSXFontInfoHeader {
    /*int         flags;
    //FSRange     char_range;
    //unsigned    draw_direction;
    //FSChar2b    default_char;
    */
    FSXCharInfo min_bounds;
    FSXCharInfo max_bounds;
    short       font_ascent;
    short       font_descent;
} FSXFontInfoHeader;

struct fs_font {
    FSXFontInfoHeader  fontHeader;
    /*unsigned char      *propData;*/
    FSXCharInfo        *extents;
    unsigned char      *glyphs;
    int                maxenc,width,height;
    FSXCharInfo        **eindex;
    unsigned char      **gindex;
};

#endif




static char *default_font[] = {
    /* why the heck every f*cking distribution picks another
       location for these fonts ??? */
    "/usr/share/consolefonts/lat1-16.psf",
    "/usr/share/consolefonts/lat1-16.psf.gz",
    "/usr/share/consolefonts/lat1-16.psfu.gz",
    "/usr/share/kbd/consolefonts/lat1-16.psf",
    "/usr/share/kbd/consolefonts/lat1-16.psf.gz",
    "/usr/share/kbd/consolefonts/lat1-16.psfu.gz",
    "/usr/lib/kbd/consolefonts/lat1-16.psf",
    "/usr/lib/kbd/consolefonts/lat1-16.psf.gz",
    "/usr/lib/kbd/consolefonts/lat1-16.psfu.gz",
    "/lib/kbd/consolefonts/lat1-16.psf",
    "/lib/kbd/consolefonts/lat1-16.psf.gz",
    "/lib/kbd/consolefonts/lat1-16.psfu.gz",
    NULL
};


static const unsigned fs_masktab[] = {
    (1 << 7), (1 << 6), (1 << 5), (1 << 4),
    (1 << 3), (1 << 2), (1 << 1), (1 << 0),
};




class FramebufferDevice;
class FontServer
{


	const FramebufferDevice &framebufferdevice;
public:
	FontServer( const FramebufferDevice &framebufferdevice_);


static void fb_text_init1(char *font, struct fs_font **_f);
static struct fs_font* fs_consolefont(char **filename);

void fs_render_fb(unsigned char *ptr, int pitch, FSXCharInfo *charInfo, unsigned char *data);



};

}

#endif

