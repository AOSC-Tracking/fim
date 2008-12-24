/* $Id$ */
/*
 FbiStuffPdf.cpp : fim functions for decoding PDF files

 (c) 2008-2009 Michele Martone

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
 * FIXME : this code is NOT meant to be correct : in particular, memory 
 * leaks are possible in case of errors.
 * */

#include <cstdio>
#include <cstdlib>



#include <cstring>
//#include <stddef.h>
//#include <errno.h>

#include "FbiStuff.h"
#include "FbiStuffLoader.h"

//extern "C"

// we require C linkage for these symbols
//#include <poppler/goo/gtypes.h>
//#include <poppler/goo/GooString.h>
#include <poppler/poppler-config.h>
#include <poppler/PDFDoc.h>
#include <poppler/OutputDev.h>
#include <poppler/SplashOutputDev.h>
#include <poppler/splash/SplashBitmap.h>
#include <poppler/splash/SplashTypes.h>
#include <poppler/Page.h>
#include <poppler/GlobalParams.h>	/* globalParams lives here */




/*								*/

namespace fim
{

/* ---------------------------------------------------------------------- */
/* load                                                                   */

struct pdf_state_t {
	SplashBitmap* bmp ;
	PDFDoc *            _pdfDoc ;
	SplashOutputDev *   _outputDev ;
	int row_stride;    /* physical row width in output buffer */
	unsigned char * first_row_dst;
};


/* ---------------------------------------------------------------------- */

static SplashColor splashColRed;
static SplashColor splashColGreen;
static SplashColor splashColBlue;
static SplashColor splashColWhite;
static SplashColor splashColBlack;
static SplashColorMode gSplashColorMode = splashModeBGR8;
#define SPLASH_COL_RED_PTR (SplashColorPtr)&(splashColRed[0])
#define SPLASH_COL_GREEN_PTR (SplashColorPtr)&(splashColGreen[0])
#define SPLASH_COL_BLUE_PTR (SplashColorPtr)&(splashColBlue[0])
#define SPLASH_COL_WHITE_PTR (SplashColorPtr)&(splashColWhite[0])
#define SPLASH_COL_BLACK_PTR (SplashColorPtr)&(splashColBlack[0])

static SplashColorPtr  gBgColor = SPLASH_COL_WHITE_PTR;

static void splashColorSet(SplashColorPtr col, Guchar red, Guchar green, Guchar blue, Guchar alpha)
{
    switch (gSplashColorMode)
    {
        case splashModeBGR8:
            col[0] = blue;
            col[1] = green;
            col[2] = red;
            break;
        case splashModeRGB8:
            col[0] = red;
            col[1] = green;
            col[2] = blue;
            break;
        default:
            assert(0);
            break;
    }
}

void SplashColorsInit(void)
{
    splashColorSet(SPLASH_COL_RED_PTR, 0xff, 0, 0, 0);
    splashColorSet(SPLASH_COL_GREEN_PTR, 0, 0xff, 0, 0);
    splashColorSet(SPLASH_COL_BLUE_PTR, 0, 0, 0xff, 0);
    splashColorSet(SPLASH_COL_BLACK_PTR, 0, 0, 0, 0);
    splashColorSet(SPLASH_COL_WHITE_PTR, 0xff, 0xff, 0xff, 0);
}

static void*
pdf_init(FILE *fp, char *filename, unsigned int page,
	  struct ida_image_info *i, int thumbnail)
{
	struct pdf_state_t * ds=NULL;
	ds = (struct pdf_state_t*)calloc(sizeof(struct pdf_state_t),1);

	if(!ds)
		return NULL;

    	ds->first_row_dst = NULL;
	SplashColorsInit();


	GooString *fileNameStr = new GooString(filename);

	 if(!fileNameStr) return NULL;

	ds->bmp = NULL;
	ds->_pdfDoc = NULL;
	ds->_outputDev = NULL;

	// WARNING : a global variable from libpoppler! damn!!
	globalParams = new GlobalParams();
	if (!globalParams)
		return NULL;

	globalParams->setErrQuiet(gFalse);
	char _[1];_[0]='\0';
	globalParams->setBaseDir(_);


	ds->_pdfDoc = new PDFDoc(fileNameStr, NULL, NULL, (void*)NULL);
	if (!ds->_pdfDoc)
        	return NULL;
	if (!ds->_pdfDoc->isOk())
		return NULL;

	if (!ds->_outputDev)
	{
        	GBool bitmapTopDown = gTrue;
        	ds->_outputDev = new SplashOutputDev(gSplashColorMode, /*4*/3, gFalse, gBgColor, bitmapTopDown,gFalse/*antialias*/);
	        if (ds->_outputDev)
			ds->_outputDev->startDoc(ds->_pdfDoc->getXRef());
    	}
        if (!ds->_outputDev)
	return NULL;

	i->dpi    = 72; /* FIXME */
	int rotation=0,pageNo=1;
	double zoomReal=250.0*2;

	int PDF_FILE_DPI = i->dpi;
	double hDPI = (double)PDF_FILE_DPI * zoomReal * 0.01;
	double vDPI = (double)PDF_FILE_DPI * zoomReal * 0.01;
	GBool  useMediaBox = gFalse;
	GBool  crop        = gTrue;
	GBool  doLinks     = gTrue;

	ds->_pdfDoc->displayPage(ds->_outputDev, pageNo, hDPI, vDPI, rotation, useMediaBox, crop, doLinks, NULL, NULL);

	if(!ds->_pdfDoc)		return NULL;

	ds->bmp = ds->_outputDev->takeBitmap();
	if(!ds->bmp) return NULL;

	i->width  = ds->bmp->getWidth();
	i->height = ds->bmp->getHeight();
	i->npages = ds->_pdfDoc->getNumPages();

	if(!ds) return NULL;
	if(fp) fclose(fp);

	return ds;
	err:
	if(ds)free(ds);
	return NULL;
}

static void
pdf_read(unsigned char *dst, unsigned int line, void *data)
{
    	struct pdf_state_t *ds = (struct pdf_state_t*)data;
	if(!ds)return;

    	if(ds->first_row_dst == NULL)
    		ds->first_row_dst = dst;
	else return;

	memcpy(dst,ds->bmp->getDataPtr(),ds->bmp->getHeight()*ds->bmp->getWidth()*3);
}

static void
pdf_done(void *data)
{
    	struct pdf_state_t *ds = (struct pdf_state_t*)data;
	if(!ds) return;


	free(ds);
}

/*
0000000: 2550 4446 2d31 2e34 0a25 d0d4 c5d8 0a35  %PDF-1.4.%.....5
*/
static struct ida_loader pdf_loader = {
    magic: "%PDF-",// FIXME : are sure this is enough ?
    moff:  0,
    mlen:  5,
    name:  "libpoppler",
    init:  pdf_init,
    read:  pdf_read,
    done:  pdf_done,
};

static void __init init_rd(void)
{
    load_register(&pdf_loader);
}

}
