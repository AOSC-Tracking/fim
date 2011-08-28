/* $LastChangedDate: 2011-06-29 16:19:20 +0200 (Wed, 29 Jun 2011) $ */
/*
 FbiStuffMagick.cpp : fim functions for decoding image files using libGraphicsMagick

 (c) 2011-2011 Michele Martone

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

/* FIXME: This code is very inefficient, so please regard it as temporary */
/* FIXME: Error handling is incomplete */
/* TODO: CatchException fprintf's stuff: it is not adequate error reporting; we would prefer an error string instead  */
/* Tested with MagickLibVersion defined as 0x090600 */

#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "../config.h"
#include "fim_types.h"
#include "FbiStuffList.h"
#include "FbiStuffLoader.h"

#ifdef HAVE_LIBGRAPHICSMAGICK
#include <magick/api.h>

struct magick_state_t {
	Image * image; /* Warning: this is NOT to be confused with fim's Image class */
	Image * cimage; /* Warning: this is NOT to be confused with fim's Image class */
	ImageInfo *image_info;
	MagickPassFail mpf; 
	ExceptionInfo exception;
};

static struct magick_state_t ms;

namespace fim
{

static void magick_cleanup()
{
	if(ms.image)DestroyImageList(ms.image);
	if(ms.cimage)DestroyImageList(ms.cimage);
	if(ms.image_info)DestroyImageInfo(ms.image_info);
	//DestroyExceptionInfo(ms.exception);
	DestroyMagick();
}

static void*
magick_init(FILE *fp, char *filename, unsigned int page,
	  struct ida_image_info *i, int thumbnail)
{
	bzero(&ms,sizeof(ms));
	ms.mpf=MagickFail;

	if(!fp && !filename)
		goto err;
	if(!filename)
		goto err;
	if(!fp)
		goto err;

	InitializeMagick(filename);
	GetExceptionInfo(&ms.exception);
	if (ms.exception.severity != UndefinedException)
	{
        	//CatchException(&ms.exception);
		goto err;
	}
	ms.image_info=CloneImageInfo((ImageInfo *) NULL);

	/* FIXME need correctness check on dimensions values ! */
	if(strlen(filename)>MaxTextExtent-1)
		goto err;
	(void) strncpy(ms.image_info->filename,filename,MaxTextExtent-1);
	ms.image_info->filename[MaxTextExtent-1]='\0';
	ms.image=ReadImage(ms.image_info,&ms.exception);
	if (ms.exception.severity != UndefinedException)
	{
        	//CatchException(&ms.exception);
		goto err;
	}
	if(!ms.image)
		goto err;
	i->npages = GetImageListLength(ms.image); /* ! */
	i->width  = (ms.image)->columns;
	i->height = (ms.image)->rows;

	if(page>=i->npages || page<0)goto err;

	if(i->width<1 || i->height<1 || i->npages<1)
		goto err;

	ms.cimage=GetImageFromList(ms.image,page);
	if (ms.exception.severity != UndefinedException)
	{
 		//CatchException(&ms.exception);
	}
	if(!ms.cimage)
	{
		goto err;
	}
	return &ms;
err:
	magick_cleanup();
	return NULL; 
}

static void
magick_read(fim_byte_t *dst, unsigned int line, void *data)
{
	/* FIXME: this is extremely inefficient */
	for(unsigned int c=0;c<(ms.cimage)->columns;++c)
	{
		PixelPacket pp=AcquireOnePixel( ms.cimage, c, line, &ms.exception );
		dst[c*3+0]=pp.red;
		dst[c*3+1]=pp.green;
		dst[c*3+2]=pp.blue;
	}
}

static void
magick_done(void *data)
{
	magick_cleanup();
}

/*
 not a filetype-specific decoder
*/
struct ida_loader magick_loader = {
    /*magic:*/ "",
    /*moff:*/  0,
    /*mlen:*/  0,
    /*name:*/  "libGraphicsMagick",
    /*init:*/  magick_init,
    /*read:*/  magick_read,
    /*done:*/  magick_done,
};

static void __init init_rd(void)
{
    fim_load_register(&magick_loader);
}

}
#endif // ifdef HAVE_LIBGRAPHICSMAGICK
