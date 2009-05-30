/* $Id: FbiStuffPdf.cpp 224 2009-03-06 00:12:20Z dezperado $ */
/*
 FbiStuffPdf.cpp : fim functions for decoding PDF files

 (c) 2009 Michele Martone
 based on code (c) 1998-2006 Gerd Knorr <kraxel@bytesex.org>

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
 * this code should be fairly correct, although unfinished
 * */

#include <cstdio>
#include <cstdlib>
#include <cstring>

#include "FbiStuff.h"
#include "FbiStuffLoader.h"

#ifdef HAVE_MATRIX_MARKET_DECODER

/* This is an experimental library of mine */
#include <vbr.h>
#include <util.h>

namespace fim
{

/* ---------------------------------------------------------------------- */
/* load                                                                   */

struct mm_state_t {
	char * filename;
	unsigned char * first_row_dst;
};


/* ---------------------------------------------------------------------- */

static void*
mm_init(FILE *fp, char *filename, unsigned int page,
	  struct ida_image_info *i, int thumbnail)
{
	size_t rows,cols;
	struct mm_state_t *h;
	h = (struct mm_state_t *)calloc(sizeof(*h),1);
	if(!h)goto err;
    	h->first_row_dst=NULL;

	h->filename=NULL;
	i->dpi    = 72; /* FIXME */
	i->npages = 1; // uhm

	if(vbr_util_get_matrix_dimensions(filename, &cols, &rows))
		goto err;

	i->width  = cols;
	i->height = rows;

	h->filename=dupstr(filename);

	if(!h->filename)
		goto err;

	return h;
err:
	if( h ) free(h);
	return NULL;
}

static void
mm_read(unsigned char *dst, unsigned int line, void *data)
{
	struct mm_state_t *h = (struct mm_state_t*)data;

	if(!h)return;
    	if(h->first_row_dst == NULL)
    		h->first_row_dst = dst;
	else
		return;

	if(vbr_get_pixmap_RGBA_from_matrix(h->filename, dst))
		goto err;
err:
	return;
}

static void
mm_done(void *data)
{
	struct mm_state_t *h = (struct mm_state_t*)data;
	if(!data)
		goto err;
	if(h->filename)
		free(h->filename);
err:
	return;
}

/*
00000000  25 25 4D 61 74 72 69 78 4D 61 72 6B 65 74 20 6D 61 74 72 69 78 20 63 6F 6F 72 64 69 %%MatrixMarket matrix coordi
*/
static struct ida_loader mm_loader = {
    magic: "%%MatrixMarket matrix",
    moff:  0,
    mlen:  20,
    name:  "matrix market",
    init:  mm_init,
    read:  mm_read,
    done:  mm_done,
};

static void __init init_rd(void)
{
    load_register(&mm_loader);
}

}

#endif // ifdef HAVE_MATRIX_MARKET_DECODER

