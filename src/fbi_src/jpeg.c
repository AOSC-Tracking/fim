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
#ifndef FIM_NO_FBI
#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include <jpeglib.h>
#ifdef HAVE_LIBEXIF
# include <libexif/exif-data.h>
#endif

#include "loader.h"
#include "misc.h"

#ifdef USE_X11
# include <X11/Xlib.h>
# include <X11/Intrinsic.h>
# include <Xm/Xm.h>
# include <Xm/Text.h>
# include <Xm/SelectioB.h>
# include "RegEdit.h"
# include "ida.h"
# include "viewer.h"
#endif

/* ---------------------------------------------------------------------- */
/* load                                                                   */

struct jpeg_state {
    FILE * infile;                /* source file */
    
    struct jpeg_decompress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPARRAY buffer;            /* Output row buffer */
    int row_stride,linelength;    /* physical row width in output buffer */
    unsigned char *image,*ptr;

    /* thumbnail */
    unsigned char  *thumbnail;
    unsigned int   tpos, tsize;
};

/* ---------------------------------------------------------------------- */
/* data source manager for thumbnail images                               */

static void thumbnail_src_init(struct jpeg_decompress_struct *cinfo)
{
    struct jpeg_state *h  = container_of(cinfo, struct jpeg_state, cinfo);
    cinfo->src->next_input_byte = h->thumbnail;
    cinfo->src->bytes_in_buffer = h->tsize;
}

static int thumbnail_src_fill(struct jpeg_decompress_struct *cinfo)
{
    fprintf(stderr,"jpeg: panic: no more thumbnail input data\n");
    exit(1);
}

static void thumbnail_src_skip(struct jpeg_decompress_struct *cinfo,
			       long num_bytes)
{
    cinfo->src->next_input_byte += num_bytes;
}

static void thumbnail_src_term(struct jpeg_decompress_struct *cinfo)
{
    /* nothing */
}

static struct jpeg_source_mgr thumbnail_mgr = {
    .init_source         = thumbnail_src_init,
    .fill_input_buffer   = thumbnail_src_fill,
    .skip_input_data     = thumbnail_src_skip,
    .resync_to_restart   = jpeg_resync_to_restart,
    .term_source         = thumbnail_src_term,
};

/* ---------------------------------------------------------------------- */
/* jpeg loader                                                            */

static void*
jpeg_init(FILE *fp, char *filename, unsigned int page,
	  struct ida_image_info *i, int thumbnail)
{
    struct jpeg_state *h;
    jpeg_saved_marker_ptr mark;
    
    h = malloc(sizeof(*h));
    memset(h,0,sizeof(*h));
    h->infile = fp;

    h->cinfo.err = jpeg_std_error(&h->jerr);
    jpeg_create_decompress(&h->cinfo);
    jpeg_save_markers(&h->cinfo, JPEG_COM,    0xffff); /* comment */
    jpeg_save_markers(&h->cinfo, JPEG_APP0+1, 0xffff); /* EXIF */
    jpeg_stdio_src(&h->cinfo, h->infile);
    jpeg_read_header(&h->cinfo, TRUE);

    for (mark = h->cinfo.marker_list; NULL != mark; mark = mark->next) {
	switch (mark->marker) {
	case JPEG_COM:
	    if (debug)
		fprintf(stderr,"jpeg: comment found (COM marker) [%.*s]\n",
			(int)mark->data_length, mark->data);
	    load_add_extra(i,EXTRA_COMMENT,mark->data,mark->data_length);
	    break;
	case JPEG_APP0 +1:
	    if (debug)
		fprintf(stderr,"jpeg: exif data found (APP1 marker)\n");
	    load_add_extra(i,EXTRA_COMMENT,mark->data,mark->data_length);

#ifdef HAVE_LIBEXIF
	    if (thumbnail) {
		ExifData *ed;
		
		ed = exif_data_new_from_data(mark->data,mark->data_length);
		if (ed->data &&
		    ed->data[0] == 0xff &&
		    ed->data[1] == 0xd8) {
		    if (debug)
			fprintf(stderr,"jpeg: exif thumbnail found\n");

		    /* save away thumbnail data */
		    h->thumbnail = malloc(ed->size);
		    h->tsize = ed->size;
		    memcpy(h->thumbnail,ed->data,ed->size);
		}
		exif_data_unref(ed);
	    }
#endif
	    break;
	}
    }

    if (h->thumbnail) {
	/* save image size */
	i->thumbnail   = 1;
	i->real_width  = h->cinfo.image_width;
	i->real_height = h->cinfo.image_height;

	/* re-setup jpeg */
	jpeg_destroy_decompress(&h->cinfo);
	fclose(h->infile);
	h->infile = NULL;
	jpeg_create_decompress(&h->cinfo);
	h->cinfo.src = &thumbnail_mgr;
	jpeg_read_header(&h->cinfo, TRUE);
    }

    h->cinfo.out_color_space = JCS_RGB;
    jpeg_start_decompress(&h->cinfo);
    i->width  = h->cinfo.image_width;
    i->height = h->cinfo.image_height;
    i->npages = 1;
    switch (h->cinfo.density_unit) {
    case 0: /* unknown */
	break;
    case 1: /* dot per inch */
	i->dpi = h->cinfo.X_density;
	break;
    case 2: /* dot per cm */
	i->dpi = res_cm_to_inch(h->cinfo.X_density);
	break;
    }

    return h;
}

static void
jpeg_read(unsigned char *dst, unsigned int line, void *data)
{
    struct jpeg_state *h = data;
    JSAMPROW row = dst;
    jpeg_read_scanlines(&h->cinfo, &row, 1);
}

static void
jpeg_done(void *data)
{
    struct jpeg_state *h = data;
    jpeg_destroy_decompress(&h->cinfo);
    if (h->infile)
	fclose(h->infile);
    if (h->thumbnail)
	free(h->thumbnail);
    free(h);
}

static struct ida_loader jpeg_loader = {
    magic: "\xff\xd8",
    moff:  0,
    mlen:  2,
    name:  "libjpeg",
    init:  jpeg_init,
    read:  jpeg_read,
    done:  jpeg_done,
};

static void __init init_rd(void)
{
    load_register(&jpeg_loader);
}

#ifdef USE_X11

/* ---------------------------------------------------------------------- */
/* jpeg writer                                                            */

static Widget jpeg_shell;
static Widget jpeg_text;
static int jpeg_quality = 75;

static void
jpeg_button_cb(Widget widget, XtPointer clientdata, XtPointer call_data)
{
    XmSelectionBoxCallbackStruct *cb = call_data;

    if (XmCR_OK == cb->reason) {
	jpeg_quality = atoi(XmTextGetString(jpeg_text));
	do_save_print();
    }
    XtUnmanageChild(jpeg_shell);
}

static int
jpeg_conf(Widget parent, struct ida_image *img)
{
    char tmp[32];
    
    if (!jpeg_shell) {
	/* build dialog */
	jpeg_shell = XmCreatePromptDialog(parent,"jpeg",NULL,0);
	XmdRegisterEditres(XtParent(jpeg_shell));
	XtUnmanageChild(XmSelectionBoxGetChild(jpeg_shell,XmDIALOG_HELP_BUTTON));
        jpeg_text = XmSelectionBoxGetChild(jpeg_shell,XmDIALOG_TEXT);
	XtAddCallback(jpeg_shell,XmNokCallback,jpeg_button_cb,NULL);
	XtAddCallback(jpeg_shell,XmNcancelCallback,jpeg_button_cb,NULL);
    }
    sprintf(tmp,"%d",jpeg_quality);
    XmTextSetString(jpeg_text,tmp);
    XtManageChild(jpeg_shell);
    return 0;
}

static int
jpeg_write(FILE *fp, struct ida_image *img)
{
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    unsigned char *line;
    unsigned int i;

    cinfo.err = jpeg_std_error(&jerr);
    jpeg_create_compress(&cinfo);
    jpeg_stdio_dest(&cinfo, fp);
    cinfo.image_width  = img->i.width;
    cinfo.image_height = img->i.height;
    if (img->i.dpi) {
	cinfo.density_unit = 1;
	cinfo.X_density = img->i.dpi;
	cinfo.Y_density = img->i.dpi;
    }
    cinfo.input_components = 3;
    cinfo.in_color_space = JCS_RGB;
    jpeg_set_defaults(&cinfo);
    jpeg_set_quality(&cinfo, jpeg_quality, TRUE);
    jpeg_start_compress(&cinfo, TRUE);

    for (i = 0, line = img->data; i < img->i.height; i++, line += img->i.width*3)
        jpeg_write_scanlines(&cinfo, &line, 1);
    
    jpeg_finish_compress(&(cinfo));
    jpeg_destroy_compress(&(cinfo));
    return 0;
}

struct ida_writer jpeg_writer = {
    label:  "JPEG",
    ext:    { "jpg", "jpeg", NULL},
    write:  jpeg_write,
    conf:   jpeg_conf,
};

static void __init init_wr(void)
{
    write_register(&jpeg_writer);
}

#endif
#endif
