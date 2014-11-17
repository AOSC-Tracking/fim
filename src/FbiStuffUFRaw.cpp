/* $LastChangedDate$ */
/*
 FbiStuffNef.cpp : fbi functions for NEF files, modified for fim

 (c) 2014-2014 Michele Martone
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
 # cp ufraw.h <and others .. :( > /usr/local/include
 # cp libufraw.a /usr/local/lib
 ./configure UFRAW_LIBS="-lglib-2.0 -L /usr/local/lib -lufraw  -lbz2 -ljasper -lexiv2"  UFRAW_CXXFLAGS="-I/usr/include/glib-2.0 -I/usr/lib/i386-linux-gnu/glib-2.0/include -I/usr/local/include" ...
 * */

#include "fim.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#ifdef HAVE_ENDIAN_H
# include <endian.h>
#endif /* HAVE_ENDIAN_H */

#include <ufraw.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#define FIM_NEF_PRINTF printf /* */
namespace fim
{

/* ---------------------------------------------------------------------- */

typedef unsigned int   uint32;
typedef unsigned short uint16;

/* ---------------------------------------------------------------------- */
/* load                                                                   */

extern "C"
{
	/* required --- with ufraw-0.19.2 */
	void ufraw_messenger(char *message, void *parentWindow) { }
	char *ufraw_binary;
	static gboolean silentMessenger;
}

static int fim_ufraw_row_writer(ufraw_data *uf,  void*volatile outp, void*pixbufp, int row, int width, int height, int grayscale, int bitDepth)
{
	/* on the example of ufraw's ppm_row_writer */
    	int rowStride = width * (grayscale ? 1 : 3) * (bitDepth > 8 ? 2 : 1);
	unsigned char * out = (unsigned char*) outp, * pixbuf = (unsigned char*) pixbufp;
	int i;

	FIM_NEF_PRINTF("out:%p row: %d, width:%d, height:%d, grayscale:%d\n",out,row,width,height,grayscale);

	if (bitDepth > 8)
	{
        	guint16 *pixbuf16 = (guint16 *)pixbuf;
        	for (i = 0; i < 3 * width * height; i++)
			pixbuf16[i] = g_htons(pixbuf16[i]);
	}
	for (i = 0; i < height; i++) {
        	memcpy(
				rowStride * row +  
				out    + i * width * (bitDepth > 8 ? 6 : 3), 
				pixbuf + i * width * (bitDepth > 8 ? 6 : 3), 
				rowStride);
            /* ufraw_set_error(uf, _("Error creating file '%s'."), uf->conf->outputFilename);
            ufraw_set_error(uf, g_strerror(errno));
            return UFRAW_ERROR; */
	}
    	return UFRAW_SUCCESS;
}

struct nef_state {
	ufraw_data *uf = NULL;
        UFRectangle Crop;
    	uint32 width;
    	uint32 height;
};

static void*
nef_init(FILE *fp, const fim_char_t *filename_, unsigned int page,
	 struct ida_image_info *i, int thumbnail)
{
    	struct nef_state *h = NULL;
    	int status;
	conf_data rc, cmd, conf;
	int optInd;
	void * pixel_region = NULL;
	int argc = 0;
	char **argv = NULL;
	fim_char_t *filename = strdupa(filename_);

	FIM_NEF_PRINTF("NEF loading\n");

    	h = (struct nef_state *)fim_calloc(1,sizeof(*h));
    	if(!h)goto oops;

    	/* uf_win32_locale_free(filename); */
    	conf_load(&rc, NULL);
    	if (rc.interpolation == half_interpolation)
		rc.interpolation = ahd_interpolation;
    	conf_copy_save(&rc, &conf_default);
    	g_strlcpy(rc.outputPath, "", max_path);
    	g_strlcpy(rc.inputFilename, "", max_path);
    	g_strlcpy(rc.outputFilename, "", max_path);
	optInd = ufraw_process_args(&argc, &argv, &cmd, &rc);
    	silentMessenger = cmd.silent;
    	conf_file_load(&conf, cmd.inputFilename);
	FIM_NEF_PRINTF("ufraw_open\n");
        h->uf = ufraw_open(filename);
	FIM_NEF_PRINTF("ufraw_config\n");
   	status = ufraw_config(h->uf, &rc, &conf, &cmd);
        if (h->uf->conf && h->uf->conf->createID == only_id && cmd.createID == -1)
		h->uf->conf->createID = no_id;
	FIM_NEF_PRINTF("ufraw_load_raw\n");
        if (status = ufraw_load_raw(h->uf) != UFRAW_SUCCESS)
       	{
            goto ret;
        }
/*
  %   cumulative   self
 time   seconds   seconds
 A profile of loading a NEF file, -O0 ... :
 58.68     37.93    37.93                             ahd_interpolate_INDI
  8.79     50.02     5.68                             develop_linear

 A profile of loading a NEF file, -O3 ... :
 38.40      8.16     8.16                             ahd_interpolate_INDI
 27.48     14.00     5.84                             develop_linear
*/
	FIM_NEF_PRINTF("ufraw_message_reset\n");
    	ufraw_message_reset(h->uf);
	FIM_NEF_PRINTF("ufraw_convert_image\n");
	status = ufraw_convert_image(h->uf); /* FIXME: this is *slow* */
	FIM_NEF_PRINTF("ufraw_get_scaled_crop\n");
    	ufraw_get_scaled_crop(h->uf, &h->Crop);
        /* based on ufraw_batch_saver/ufraw_write_image */
	h->height = h->uf->initialHeight, h->width = h->uf->initialWidth;
    	i->width  = h->width;
    	i->height = h->height;
	i->dpi = 0; /* FIXME */
    	i->npages = 1;
ret:
    	return h;
oops:
    	if(h)
		fim_free(h);
    	return NULL;
}

static void
nef_read(fim_byte_t *dst, unsigned int line, void *data)
{
    	struct nef_state *h = (struct nef_state *) data;
    	static int fim_nef_read_done = 0;

    	if( fim_nef_read_done == 0 )
	{
    		fim_nef_read_done++;
	    	FIM_NEF_PRINTF("%d x %d\n",h->uf->initialHeight, h->uf->initialWidth);
        	ufraw_write_image_data(h->uf, dst, &h->Crop, 8, 0, fim_ufraw_row_writer); /* FIXME: bpp = 8 here */
	}
}

static void
nef_done(void *data)
{
    	struct nef_state *h = (struct nef_state *) data;
    	fim_free(h);
	FIM_NEF_PRINTF("NEF loaded\n");
}

/*static*/ struct ida_loader nef_loader = {
    /*magic:*/ "MM\0",
    /*moff:*/  0,
    /*mlen:*/  3,
    /*name:*/  "nef",
    /*init:*/  nef_init,
    /*read:*/  nef_read,
    /*done:*/  nef_done,
};

static void __init init_rd(void)
{
    fim_load_register(&nef_loader);
}

}

