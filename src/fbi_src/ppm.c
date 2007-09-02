/*
     (c) 2007 Michele Martone
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
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "loader.h"
#ifdef USE_X11
# include "viewer.h"
#endif

/* ---------------------------------------------------------------------- */
/* load                                                                   */

struct ppm_state {
    FILE          *infile;
    int           width,height;
    unsigned char *row;
};

static void*
pnm_init(FILE *fp, char *filename, unsigned int page,
	 struct ida_image_info *i, int thumbnail)
{
    struct ppm_state *h;
    char line[1024];

    h = malloc(sizeof(*h));
    memset(h,0,sizeof(*h));

    h->infile = fp;
    fgets(line,sizeof(line),fp); /* Px */
    fgets(line,sizeof(line),fp); /* width height */
    while ('#' == line[0])
	fgets(line,sizeof(line),fp); /* skip comments */
    sscanf(line,"%d %d",&h->width,&h->height);
    fgets(line,sizeof(line),fp); /* ??? */
    if (0 == h->width || 0 == h->height)
	goto oops;
    i->width  = h->width;
    i->height = h->height;
    i->npages = 1;
    h->row = malloc(h->width*3);

    return h;

 oops:
    fclose(fp);
    free(h);
    return NULL;
}

static void
ppm_read(unsigned char *dst, unsigned int line, void *data)
{
    struct ppm_state *h = data;

    fread(dst,h->width,3,h->infile);
}

static void
pgm_read(unsigned char *dst, unsigned int line, void *data)
{
    struct ppm_state *h = data;
    unsigned char *src;
    int x;

    fread(h->row,h->width,1,h->infile);
    src = h->row;
    for (x = 0; x < h->width; x++) {
	dst[0] = src[0];
	dst[1] = src[0];
	dst[2] = src[0];
	dst += 3;
	src += 1;
    }
}

static void
pnm_done(void *data)
{
    struct ppm_state *h = data;

    fclose(h->infile);
    free(h->row);
    free(h);
}

struct ida_loader ppm_loader = {
    magic: "P6",
    moff:  0,
    mlen:  2,
    name:  "ppm parser",
    init:  pnm_init,
    read:  ppm_read,
    done:  pnm_done,
};

static struct ida_loader pgm_loader = {
    magic: "P5",
    moff:  0,
    mlen:  2,
    name:  "pgm parser",
    init:  pnm_init,
    read:  pgm_read,
    done:  pnm_done,
};

static void __init init_rd(void)
{
    load_register(&ppm_loader);
    load_register(&pgm_loader);
}

#ifdef USE_X11
/* ---------------------------------------------------------------------- */
/* save                                                                   */

static int
ppm_write(FILE *fp, struct ida_image *img)
{
    fprintf(fp,"P6\n"
	    "# written by ida " VERSION "\n"
	    "# http://bytesex.org/ida/\n"
	    "%d %d\n255\n",
            img->i.width,img->i.height);
    fwrite(img->data, img->i.height, 3*img->i.width, fp);
    return 0;
}

static struct ida_writer ppm_writer = {
    label:  "PPM",
    ext:    { "ppm", NULL},
    write:  ppm_write,
};

static void __init init_wr(void)
{
    write_register(&ppm_writer);
}
#endif
