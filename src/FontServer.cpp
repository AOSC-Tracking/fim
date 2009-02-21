/* $Id$ */
/*
 FontServer.cpp : Font Server code from fbi, adapted for fim.

 (c) 2007-2009 Michele Martone
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

	FontServer::FontServer( )
	{
	}


#if 1
/* 20080507 unused ? */
void FontServer::fb_text_init1(const char *font, struct fs_font **_f)
{
    const char   *fonts[2] = { font, NULL };

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

#if 1
/* 20080507 unused, as fs_consolefont ? */
/* see src/FontServer.cpp : it is untrue that it is not used ! */
static const char *default_font[] = {
    /* why the heck every f*cking distribution picks another
       location for these fonts ??? (GK) */
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
#endif


struct fs_font* FontServer::fs_consolefont(const char **filename)
{
    int  i;
    int  fr;
    const char *h;
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
	fp = FbiStuff::fim_execlp("zcat","zcat",filename[i],NULL);
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
    fr=fread(f->glyphs, 256, f->height, fp);
    if(!fr)return NULL;/* new */
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


