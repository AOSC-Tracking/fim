/* $Id$ */
/*
 FramebufferDevice.h : Linux Framebuffer functions from fbi, adapted for fim

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
#ifdef FIM_NO_FBI

#ifndef FRAMEBUFFER_DEVICE_FIM_H
#define FRAMEBUFFER_DEVICE_FIM_H

/*
 *
 * 20080106
 *
 * This code is still embryonal, and aims at replacing the whole core of 
 * fbi code and functionality (concerning framebuffer device stuff) in fim.
 *
 * So don't be surprised if the following code is deactivated via the preprocessor.
 *
 * */



#include "fim.h"
#include "FbiStuff.h"
#include "FontServer.h"
#include "FbiStuffFbtools.h"
#include <stdio.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <linux/vt.h>
#include <errno.h>
#include <sys/mman.h>
#include <math.h> //powe
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>
#include <sys/ioctl.h>
#include <sys/wait.h>
#include <sys/stat.h>
*/
/*
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <termios.h>
#include <signal.h>
#include <errno.h>
#include <setjmp.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/wait.h>
#include <sys/stat.h>

#include <linux/fb.h>


*/
#include <linux/vt.h>
#include <sys/user.h>	// PAGE_MASK

#include <linux/kd.h>
#include <linux/fb.h>	// fb_fix_screeninfo






/* from fbtools.h */
#define FB_ACTIVE    0
#define FB_REL_REQ   1
#define FB_INACTIVE  2
#define FB_ACQ_REQ   3


#define TRUE            1
#define FALSE           0

namespace fim
{
struct DEVS {
    char *fb0;
    char *fbnr;
    char *ttynr;
};



void _fb_switch_signal(int signal);



 


#define DITHER_LEVEL 8

static long     red_mult, green_mult;
static long     red_dither[256];
static long     green_dither[256];
static long     blue_dither[256];
static long     gray_dither[256];

typedef unsigned long vector[DITHER_LEVEL];
typedef vector  matrix[DITHER_LEVEL];




//#if DITHER_LEVEL == 8 ||  DITHER_LEVEL == 4
//static int matrix   DM ;
//#endif
#if DITHER_LEVEL == 8
#define DITHER_MASK 7
static matrix   DM =
{
    {0, 32, 8, 40, 2, 34, 10, 42},
    {48, 16, 56, 24, 50, 18, 58, 26},
    {12, 44, 4, 36, 14, 46, 6, 38},
    {60, 28, 52, 20, 62, 30, 54, 22},
    {3, 35, 11, 43, 1, 33, 9, 41},
    {51, 19, 59, 27, 49, 17, 57, 25},
    {15, 47, 7, 39, 13, 45, 5, 37},
    {63, 31, 55, 23, 61, 29, 53, 21}
};

#endif

#if DITHER_LEVEL == 4
#define DITHER_MASK 3
static matrix   DM =
{
    {0, 8, 2, 10},
    {12, 4, 14, 6},
    {3, 11, 1, 9},
    {15, 7, 13, 5}
};

#endif




class FramebufferDevice
{
	/*
	 * 20080106
	 * An UNFINISHED class providing access to a single framebuffer device.
	 *
	 *
	 * Let's say in future we want to be able to manage multiple framebuffer devices.
	 * Then framebuffer variables should be incapsulated well in separate objects.
	 * We are heading forward on this road.
	 * */

#if 0
	void fb_text_init1(char *font)
	{
	    char   *fonts[2] = { font, NULL };
	
	    if (NULL == f)
		f = fs_consolefont(font ? fonts : NULL);
	#ifndef X_DISPLAY_MISSING
	    if (NULL == f && 0 == fs_connect(NULL))
		f = fs_open(font ? font : x11_font);
	#endif
	    if (NULL == f) {
		fprintf(stderr,"font \"%s\" is not available\n",font);
		exit(1);
	    }
	}
#endif
	private:
	FontServer fontserver;
	char* fontname;
	/*
	struct fb_fix_screeninfo   fb_fix;
	struct fb_var_screeninfo   fb_var;
	unsigned char             *fb_mem;
	int			   fb_mem_offset = 0;
	int                        fb_switch_state = FB_ACTIVE;
	*/

	public:
	int              vt ;
	int32_t         lut_red[256], lut_green[256], lut_blue[256];
	int             dither , pcd_res , steps ;

	/*static float fbgamma = 1;*/
	float fbgamma ;


	// FS.C
	unsigned int       fs_bpp, fs_black, fs_white;//STILL UNINITIALIZED
	int fs_init_fb(int white8);
	private:
	/* public */
	int visible ;

	/* private */
	struct fs_font *f;
	char *x11_font ;

	int ys ;
	int xs ;

	public:
	void (*fs_setpixel)(void *ptr, unsigned int color);
	private:

static void setpixel1(void *ptr, unsigned int color)
{
    unsigned char *p = (unsigned char *) ptr;
    *p = color;
}
static void setpixel2(void *ptr, unsigned int color)
{
    unsigned char *p = (unsigned char *) ptr;
    *p = color;
}
static void setpixel3(void *ptr, unsigned int color)
{
    unsigned char *p = (unsigned char *) ptr;
    *(p++) = (color >> 16) & 0xff;
    *(p++) = (color >>  8) & 0xff;
    *(p++) =  color        & 0xff;
}
static void setpixel4(void *ptr, unsigned int color)
{
    unsigned char *p = (unsigned char *) ptr;
    *p = color;
}





	// FBI.C
	/* framebuffer */
	public:
	char                       *fbdev;
	char                       *fbmode;
	//private:
	int                        fd, switch_last, debug;
	int                        redraw;

	unsigned short red[256],  green[256],  blue[256];
	struct fb_cmap cmap;


	//were static ..
	struct fb_cmap            ocmap;
	unsigned short            ored[256], ogreen[256], oblue[256];


	struct DEVS devs_default;
	struct DEVS devs_devfs;


	// FBTOOLS.C
	FramebufferDevice():
	fontname(NULL),
	visible(1),
	x11_font("10x20"),
	ys( 3),
	xs(10),
	fb_mem_offset(0),
	fb_switch_state(FB_ACTIVE),
	orig_vt_no(0),
	fbdev(NULL),
	fbmode(NULL),
	fbgamma(1.0),
	vt(0),
	dither(FALSE),
	pcd_res(3),
	steps(50),
	debug(0),
	fs_setpixel(NULL),
	fontserver(*this)
	{
		cmap.start  =  0;
		cmap.len    =  256;
		cmap.red  =  red;
		cmap.green  =  green;
		cmap.blue  =  blue;
		//! transp!
		devs_default.fb0=   "/dev/fb0";
		devs_default.fbnr=  "/dev/fb%d";
		devs_default.ttynr= "/dev/tty%d";
		devs_devfs.fb0=   "/dev/fb/0";
		devs_devfs.fbnr=  "/dev/fb/%d";
		devs_devfs.ttynr= "/dev/vc/%d";
		ocmap.start = 0;
		ocmap.len   = 256;
		ocmap.red=ored;
		ocmap.green=ogreen;
		ocmap.blue=oblue;
		/*
		 * fbgamma and fontname are fbi - defined variables.
		 * */
		char *line;

	    	if (NULL != (line = getenv("FBGAMMA")))
	        	fbgamma = atof(line);
	    	if (NULL != (line = getenv("FBFONT")))
			fontname = line;
	}


/* -------------------------------------------------------------------- */
	/* exported stuff                                                       */
	public:
	struct fb_fix_screeninfo   fb_fix;
	struct fb_var_screeninfo   fb_var;
	//private:
	unsigned char             *fb_mem;
	int			   fb_mem_offset;
	int                        fb_switch_state;

/* -------------------------------------------------------------------- */
	/* internal variables                                                   */

	int                       fb,tty;
	#if 0
	static int                       bpp,black,white;
	#endif

	int                       orig_vt_no;
	struct vt_mode            vt_mode;
	int                       kd_mode;
	struct vt_mode            vt_omode;
	struct termios            term;
	struct fb_var_screeninfo  fb_ovar;


	/////////////////
	public:
	int framebuffer_init();

	struct DEVS *devices;



	void dev_init(void);
	int fb_init(char *device, char *mode, int vt);

	void fb_memset (void *addr, int c, size_t len);
	void fb_setcolor(int c) { fb_memset(fb_mem+fb_mem_offset,c,fb_fix.smem_len); }



	void fb_setvt(int vtno);
	int fb_setmode(char *name);
	static int fb_activate_current(int tty);

	void console_switch(int is_busy);

	int  fb_font_width(void);
	int  fb_font_height(void);

	void fb_status_line(unsigned char *msg);

	void fb_edit_line(unsigned char *str, int pos);

	void fb_text_box(int x, int y, char *lines[], unsigned int count);

	void fb_line(int x1, int x2, int y1,int y2);


	void fb_rect(int x1, int x2, int y1,int y2);

	void fb_setpixel(int x, int y, unsigned int color);

	int fs_puts(struct fs_font *f, unsigned int x, unsigned int y, unsigned char *str);

	void fb_clear_rect(int x1, int x2, int y1,int y2);

	void fb_clear_mem(void);
	void fb_cleanup(void);
	struct fs_font * fb_font_get_current_font(void)
	{
	    return f;
	}

	void switch_if_needed()
	{
		//fim's
		if (switch_last != fb_switch_state)
		    console_switch(1);
	}

#define TRUE            1
#define FALSE           0
#define MAX(x,y)        ((x)>(y)?(x):(y))
#define MIN(x,y)        ((x)<(y)?(x):(y))
#define ARRAY_SIZE(x)   (sizeof(x)/sizeof(x[0]))

/*
 * framebuffer memory offset for x pixels left and y right from the screen
 * (by dez)
 */
#define FB_BPP  (((fb_var.bits_per_pixel+7)/8))
#define FB_MEM_LINE_OFFSET  ((FB_BPP*fb_var.xres))
#define FB_MEM_OFFSET(x,y)  (( FB_BPP*(x) + FB_MEM_LINE_OFFSET * (y) ))
#define FB_MEM(x,y) ((fb_mem+FB_MEM_OFFSET((x),(y))))



//void svga_display_image_new(struct ida_image *img, int xoff, int yoff,unsigned int bx,unsigned int bw,unsigned int by,unsigned int bh,int mirror,int flip);
void svga_display_image_new(struct ida_image *img, int xoff, int yoff,unsigned int bx,unsigned int bw,unsigned int by,unsigned int bh,int mirror,int flip);

/* ---------------------------------------------------------------------- */
/* by dez
 */
unsigned char * clear_line(int bpp, int line, int owidth, char unsigned *dest);
unsigned char * convert_line(int bpp, int line, int owidth, char unsigned *dest, char unsigned *buffer, int mirror);/*dez's mirror patch*/







void init_dither(int shades_r, int shades_g, int shades_b, int shades_gray);
void dither_line(unsigned char *src, unsigned char *dest, int y, int width,int mirror);

void dither_line_gray(unsigned char *src, unsigned char *dest, int y, int width);


//////////////////////////
void fb_switch_release();

void fb_switch_acquire();

int fb_switch_init();

void fb_switch_signal(int signal);

 // FB-GUI
int fb_text_init2(void);

 // end FB-GUI


/*static void*/
void svga_dither_palette(int r, int g, int b)
{
    int             rs, gs, bs, i;

    rs = 256 / (r - 1);
    gs = 256 / (g - 1);
    bs = 256 / (b - 1);
    for (i = 0; i < 256; i++) {
	red[i]   = calc_gamma(rs * ((i / (g * b)) % r), 255);
	green[i] = calc_gamma(gs * ((i / b) % g),       255);
	blue[i]  = calc_gamma(bs * ((i) % b),           255);
    }
}


unsigned short calc_gamma(int n, int max)
{
    int ret =(int)(65535.0 * pow((float)n/(max), 1 / fbgamma)); 
    if (ret > 65535) ret = 65535;
    if (ret <     0) ret =     0;
    return ret;
}

void linear_palette(int bit)
{
    int i, size = 256 >> (8 - bit);
    
    for (i = 0; i < size; i++)
        red[i] = green[i] = blue[i] = calc_gamma(i,size);
}

void lut_init(int depth)
{
    if (fb_var.red.length   &&
	fb_var.green.length &&
	fb_var.blue.length) {
	/* fb_var.{red|green|blue} looks sane, use it */
	init_one(lut_red,   fb_var.red.length,   fb_var.red.offset);
	init_one(lut_green, fb_var.green.length, fb_var.green.offset);
	init_one(lut_blue,  fb_var.blue.length,  fb_var.blue.offset);
    } else {
	/* fallback */
	int i;
	switch (depth) {
	case 15:
	    for (i = 0; i < 256; i++) {
		lut_red[i]   = (i & 0xf8) << 7;	/* bits -rrrrr-- -------- */
		lut_green[i] = (i & 0xf8) << 2;	/* bits ------gg ggg----- */
		lut_blue[i]  = (i & 0xf8) >> 3;	/* bits -------- ---bbbbb */
	    }
	    break;
	case 16:
	    for (i = 0; i < 256; i++) {
		lut_red[i]   = (i & 0xf8) << 8;	/* bits rrrrr--- -------- */
		lut_green[i] = (i & 0xfc) << 3;	/* bits -----ggg ggg----- */
		lut_blue[i]  = (i & 0xf8) >> 3;	/* bits -------- ---bbbbb */
	    }
	    break;
	case 24:
	    for (i = 0; i < 256; i++) {
		lut_red[i]   = i << 16;	/* byte -r-- */
		lut_green[i] = i << 8;	/* byte --g- */
		lut_blue[i]  = i;		/* byte ---b */
	    }
	    break;
	}
    }
}

void init_one(int32_t *lut, int bits, int shift)
{
    int i;
    
    if (bits > 8)
	for (i = 0; i < 256; i++)
	    lut[i] = (i << (bits + shift - 8));
    else
	for (i = 0; i < 256; i++)
	    lut[i] = (i >> (8 - bits)) << shift;
}


};

}



#endif

#endif

