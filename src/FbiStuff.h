/* $Id$ */
/*
 FbiStuff.h : Misc fbi functionality routines, modified for fim

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

#ifndef FBI_STUFF_FBI_H
#define FBI_STUFF_FBI_H


#include "FramebufferDevice.h"
#include "FbiStuffLoader.h"
#include "FontServer.h"

#ifdef USE_X11
# include <X11/Intrinsic.h>
#endif
#include <stdio.h>//FILE
#include <stdlib.h>//free()
#include <errno.h>//free()
#include <string.h>//memset,strerror,..

// WARNING : THIS IS PURE REDUNDANCE !
/*
struct list_head {
	struct list_head *next, *prev;
};*/

extern struct list_head loaders;


//extern FramebufferDevice ffd;

namespace fim
{

extern int debug; //WARNING ! THIS IS UNINITIALIZED!





class FbiStuff{
public:
static void free_image(struct ida_image *img);
static struct ida_image* read_image(char *filename);
static struct ida_image* rotate_image(struct ida_image *src, unsigned int rotation);
static struct ida_image* scale_image(struct ida_image *src, float scale, float ascale);

static int fim_filereading_debug();
};

// filter.h

struct op_3x3_parm {
    int f1[3];
    int f2[3];
    int f3[3];
    int mul,div,add;
};

struct op_sharpe_parm {
    int factor;
};

struct op_resize_parm {
    int width;
    int height;
    int dpi;
};

struct op_rotate_parm {
    int angle;
};


//int             new_image;
// end filter.h

/* 
 * dez's function, on the way to windowing Fim!
 * */
struct ida_image * fbi_image_clone(struct ida_image *img);

}

#endif


