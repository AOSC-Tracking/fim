/* $Id$ */
/*
 FontServer.cpp : Font Server code from fbi, adapted for fim.

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



#include "fim.h"

namespace fim
{


#define BIT_ORDER       BitmapFormatBitOrderMSB
#ifdef BYTE_ORDER
#undef BYTE_ORDER
#endif
#define BYTE_ORDER      BitmapFormatByteOrderMSB
#define SCANLINE_UNIT   BitmapFormatScanlineUnit8
#define SCANLINE_PAD    BitmapFormatScanlinePad8
#define EXTENTS         BitmapFormatImageRectMin

#define SCANLINE_PAD_BYTES 1
#define GLWIDTHBYTESPADDED(bits, nBytes)                                    \
        ((nBytes) == 1 ? (((bits)  +  7) >> 3)          /* pad to 1 byte  */\
        :(nBytes) == 2 ? ((((bits) + 15) >> 3) & ~1)    /* pad to 2 bytes */\
        :(nBytes) == 4 ? ((((bits) + 31) >> 3) & ~3)    /* pad to 4 bytes */\
        :(nBytes) == 8 ? ((((bits) + 63) >> 3) & ~7)    /* pad to 8 bytes */\
        : 0)







void FontServer::fs_render_fb(unsigned char *ptr, int pitch, FSXCharInfo *charInfo, unsigned char *data)
{
    int row,bit,bpr,x;

    bpr = GLWIDTHBYTESPADDED((charInfo->right - charInfo->left),
			     SCANLINE_PAD_BYTES);
    for (row = 0; row < (charInfo->ascent + charInfo->descent); row++) {
	for (x = 0, bit = 0; bit < (charInfo->right - charInfo->left); bit++) {
	    if (data[bit>>3] & fs_masktab[bit&7])
		// WARNING !
		framebufferdevice.fs_setpixel(ptr+x,framebufferdevice.fs_white);
	    x += framebufferdevice.fs_bpp;
	}
	data += bpr;
	ptr += pitch;
    }
}
	FontServer::FontServer( const FramebufferDevice &framebufferdevice_):
		framebufferdevice(framebufferdevice_)
	{
	}


#if 1
/* 20080507 unused ? */
void FontServer::fb_text_init1(char *font, struct fs_font **_f)
{
    char   *fonts[2] = { font, NULL };

    if (NULL == *_f)
	*_f = fs_consolefont(font ? fonts : NULL);
#ifndef X_DISPLAY_MISSING
    if (NULL == *_f && 0 == fs_connect(NULL))
	*_f = fs_open(font ? font : x11_font);
#endif
    if (NULL == *_f) {
	fprintf(stderr,"font \"%s\" is not available\n",font);
	exit(1);
    }
}

struct fs_font* FontServer::fs_consolefont(char **filename)
{
    int  i;
    char *h,command[256];
    struct fs_font *f = NULL;
    FILE *fp;

    if (NULL == filename)
	filename = fim::default_font;

    for(i = 0; filename[i] != NULL; i++) {
	if (-1 == access(filename[i],R_OK))
	    continue;
	break;
    }
    if (NULL == filename[i]) {
	fprintf(stderr,"can't find console font file\n");
	return NULL;
    }

    h = filename[i]+strlen(filename[i])-3;
    if (0 == strcmp(h,".gz")) {
	sprintf(command,"zcat %s",filename[i]);
	fp = popen(command,"r");
    } else {
	fp = fopen(filename[i], "r");
    }
    if (NULL == fp) {
	fprintf(stderr,"can't open %s: %s\n",filename[i],strerror(errno));
	return NULL;
    }

    if (fgetc(fp) != 0x36 ||
	fgetc(fp) != 0x04) {
	fprintf(stderr,"can't use font %s\n",filename[i]);
	return NULL;
    }
//    fprintf(stderr,"using linux console font \"%s\"\n",filename[i]);

    f =(struct fs_font*) malloc(sizeof(*f));
    memset(f,0,sizeof(*f));
	
    fgetc(fp);
    f->maxenc = 256;
    f->width  = 8;
    f->height = fgetc(fp);
    f->fontHeader.min_bounds.left    = 0;
    f->fontHeader.max_bounds.right   = f->width;
    f->fontHeader.max_bounds.descent = 0;
    f->fontHeader.max_bounds.ascent  = f->height;

    f->glyphs  =(unsigned char*) malloc(f->height * 256);
    f->extents = (FSXCharInfo*)malloc(sizeof(FSXCharInfo)*256);
    fread(f->glyphs, 256, f->height, fp);
    fclose(fp);

    f->eindex  =(FSXCharInfo**) malloc(sizeof(FSXCharInfo*)   * 256);
    f->gindex  = (unsigned char**)malloc(sizeof(unsigned char*) * 256);
    for (i = 0; i < 256; i++) {
	f->eindex[i] = f->extents +i;
	f->gindex[i] = f->glyphs  +i * f->height;
	f->eindex[i]->left    = 0;
	f->eindex[i]->right   = 7;
	f->eindex[i]->width   = 8;
	f->eindex[i]->descent = 0;
	f->eindex[i]->ascent  = f->height;
    }
    return f;
}
#endif




}


